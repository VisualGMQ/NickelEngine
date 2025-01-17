#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/common/log.hpp"

#include <exception>

namespace nickel {

template <typename T>
class BlockMemoryAllocator {
public:
    static BlockMemoryAllocator& GetInst() {
        static BlockMemoryAllocator instance;
        return instance;
    }
    
    BlockMemoryAllocator(size_t block_mem_count = 256)
        : m_block_mem_count{block_mem_count} {}

    BlockMemoryAllocator(const BlockMemoryAllocator&) = delete;
    BlockMemoryAllocator& operator=(const BlockMemoryAllocator&) = delete;

    BlockMemoryAllocator(BlockMemoryAllocator&& o) noexcept
        : m_block_mem_count{o.m_block_mem_count}, m_block_head{o.m_block_head} {
        o.m_block_mem_count = 0;
        o.m_block_head = 0;
    }

    BlockMemoryAllocator& operator=(BlockMemoryAllocator&& o) noexcept {
        if (&o != this) {
            m_block_mem_count = o.m_block_mem_count;
            m_block_head = o.m_block_head;
            o.m_block_mem_count = 0;
            o.m_block_head = nullptr;
        }
        return *this;
    }

    template <typename... Args>
    T* Allocate(Args&&... args) noexcept {
        Block* block = ensure_block();
        if (block == nullptr) {
            return nullptr;
        }

        Node<size_t>* index_node = block->m_unused_index_head;
        Mem* mem = block->m_mem + index_node->m_data;

        T* elem;
        try {
            elem =
                std::construct_at((T*)&mem->m_mem, std::forward<Args>(args)...);
        } catch (...) {
            LOGE("catch exception when deconstruct object");
            return nullptr;
        }

        block->m_unused_index_head = block->m_unused_index_head->m_next;
        index_node->m_next = block->m_in_use_index_head;
        block->m_in_use_index_head = index_node;
        mem->m_in_use = true;
        return elem;
    }

    void Deallocate(T* p) noexcept {
        Node<Block>* block_node = m_block_head;

        while (block_node &&
               (std::uintptr_t(p) < std::uintptr_t(block_node->m_data.m_mem) ||
                std::uintptr_t(p) >=
                    std::uintptr_t(block_node->m_data.m_mem +
                                   m_block_mem_count * sizeof(Mem)))) {
            block_node = block_node->m_next;
        }

        if (block_node) {
            NICKEL_ASSERT(
                (std::ptrdiff_t(p) - std::ptrdiff_t(block_node->m_data.m_mem)) %
                        sizeof(Mem) ==
                    0,
                "invalid memory address");

            size_t idx = (Mem*)p - block_node->m_data.m_mem;
            Mem* mem = block_node->m_data.m_mem + idx;

            if (!mem->m_in_use) {
                LOGE("memory is not in use when deallocate");
                return ; 
            }

            try {
                ((T*)&mem->m_mem)->~T();
            } catch (...) {
                LOGE("catch exception when deconstruct object");
                return;
            }

            mem->m_in_use = false;
            Node<size_t>* in_use_node = block_node->m_data.m_in_use_index_head;
            block_node->m_data.m_in_use_index_head = in_use_node->m_next;
            in_use_node->m_data = idx;

            Block& block = block_node->m_data;
            in_use_node->m_next = block.m_unused_index_head;
            block.m_unused_index_head = in_use_node;
            return;
        }

        LOGE("object is not in pool");
        delete p;
    }

    // for debug
    size_t BlockCount() const noexcept {
        size_t count = 0;
        Node<Block>* block = m_block_head;
        while (block) {
            count++;
            block = block->m_next;
        }
        return count;
    }

    // for debug
    size_t UnuseCount(size_t block_index) const noexcept {
        Node<Block>* block = m_block_head;
        while (block_index > 0 && block) {
            block = block->m_next;
            --block_index;
        }

        if (block) {
            size_t count = 0;
            Node<size_t>* node = block->m_data.m_unused_index_head;
            while (node) {
                node = node->m_next;
                count++;
            }
            return count;
        }

        return 0;
    }

    // for debug
    size_t InuseCount(size_t block_index) const noexcept {
        Node<Block>* block = m_block_head;
        while (block_index > 0 && block) {
            block = block->m_next;
            --block_index;
        }

        if (block) {
            size_t count = 0;
            Node<size_t>* node = block->m_data.m_in_use_index_head;
            while (node) {
                node = node->m_next;
                count++;
            }
            return count;
        }

        return 0;
    }

    void FreeAll() noexcept {
        if (!m_block_head) {
            return;
        }

        Node<Block>* node = m_block_head;
        while (node) {
            Node<Block>* cur = node;
            node = node->m_next;
            delete cur;
        }
        m_block_head = nullptr;
    }

    ~BlockMemoryAllocator() { FreeAll(); }

private:
    struct Mem {
        unsigned char m_mem[sizeof(T)];
        bool m_in_use = false;
    };

    // NOTE: why we don't use std::list? cause move constructor of std::list is
    // not noexcept, I don't want exception slow my performance
    template <typename U>
    struct Node {
        U m_data;
        Node* m_next{};
    };

    struct Block {
        Mem* m_mem{};
        Node<size_t>* m_unused_index_head{};
        Node<size_t>* m_in_use_index_head{};

        ~Block() noexcept {
            Node<size_t>* in_use_node = m_in_use_index_head;
            while (in_use_node) {
                try {
                    ((T*)(m_mem + in_use_node->m_data)->m_mem)->~T();
                } catch (...) {
                    LOGE("catched exception when deconstruct object");
                }
                in_use_node = in_use_node->m_next;
            }

            Node<size_t>* node = m_unused_index_head;
            while (node) {
                Node<size_t>* cur = node;
                node = node->m_next;
                try {
                    delete cur;
                } catch (...) {
                    LOGE("catched exception when object deconstruct");
                }
            }
            
            delete[] m_mem;
        }
    };

    Node<Block>* m_block_head{};
    const size_t m_block_mem_count;

    Block* ensure_block() noexcept {
        Node<Block>* node = m_block_head;
        Node<Block>* prev = m_block_head;
        while (node && !node->m_data.m_unused_index_head) {
            prev = node;
            node = node->m_next;
        }

        if (!node) {
            Node<Block>* new_block = new (std::nothrow) Node<Block>;

            if (!new_block) {
                LOGE("bad alloc exception! out of memory");
                return nullptr;
            }

            new_block->m_data.m_mem = new (std::nothrow) Mem[m_block_mem_count];
            if (!new_block->m_data.m_mem) {
                LOGE("allocate memory block failed");
                return nullptr;
            }

            Node<size_t>* prev_index = new_block->m_data.m_unused_index_head;
            for (size_t i = 0; i < m_block_mem_count; i++) {
                Node<size_t>* new_index = new (std::nothrow) Node<size_t>;

                if (!new_index) {
                    LOGE("bad alloc exception! out of memory");
                    continue;
                }

                new_index->m_data = i;

                if (prev_index) {
                    prev_index->m_next = new_index;
                } else {
                    new_block->m_data.m_unused_index_head = new_index;
                }
                prev_index = new_index;
            }

            if (prev) {
                prev->m_next = new_block;
            } else {
                m_block_head = new_block;
            }
            return &new_block->m_data;
        }

        return &node->m_data;
    }
};

}  // namespace nickel