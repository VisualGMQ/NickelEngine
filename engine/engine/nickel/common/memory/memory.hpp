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
        : m_block_mem_count{block_mem_count} {
    }

    BlockMemoryAllocator(const BlockMemoryAllocator&) = delete;
    BlockMemoryAllocator& operator=(const BlockMemoryAllocator&) = delete;

    BlockMemoryAllocator(BlockMemoryAllocator&& o) noexcept
        : m_block_mem_count{o.m_block_mem_count}, m_block_head{o.m_block_head} {
        o.m_block_mem_count = 0;
        o.m_block_head = nullptr;
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

        return block->Allocate(std::forward<Args>(args)...);
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
            block_node->m_data.Deallocate(p);
        } else {
            LOGE("object is not in pool");
        }
    }

    void MarkAsGarbage(T* p) noexcept {
        Node<Block>* block_node = m_block_head;

        while (block_node &&
               (std::uintptr_t(p) < std::uintptr_t(block_node->m_data.m_mem) ||
                std::uintptr_t(p) >=
                std::uintptr_t(block_node->m_data.m_mem +
                               m_block_mem_count * sizeof(Mem)))) {
            block_node = block_node->m_next;
        }

        if (block_node) {
            block_node->m_data.MarkAsGarbage(p);
        } else {
            LOGE("object is not in pool");
        }
    }

    T* RequireReuse() noexcept {
        Node<Block>* block_node = m_block_head;
        while (block_node) {
            if (T* data = block_node->m_data.RequireReuse()) {
                return data;
            }
            block_node = block_node->m_next;
        }
        return nullptr;
    }

    template <typename... Args>
    T* ReuseOrAllocate(Args&&... args) noexcept {
        if (T* data = RequireReuse()) {
            return data;
        }

        return Allocate(std::forward<Args>(args)...);
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
            
            for (int i = 0; i < block->m_data.m_block_mem_count; i++) {
                Mem* mem = block->m_data.m_mem + i;
                if (mem->m_status == Mem::Status::InUse) {
                    count ++;
                }
            }
            return count;
        }

        return 0;
    }

    // for debug
    size_t PendingDeleteCount(size_t block_index) const noexcept {
        Node<Block>* block = m_block_head;
        while (block_index > 0 && block) {
            block = block->m_next;
            --block_index;
        }

        if (block) {
            size_t count = 0;
            Node<size_t>* node = block->m_data.m_pending_delete_head;
            while (node) {
                node = node->m_next;
                count++;
            }
            return count;
        }

        return 0;
    }

    void GC(size_t count = std::numeric_limits<size_t>::max()) noexcept {
        Node<Block>* block = m_block_head;
        while (block && count > 0) {
            count -= block->m_data.GC(count);
            block = block->m_next;
        }
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
        enum class Status {
            InUse,
            PendingDelete,
            Unuse,
        };
        
        unsigned char m_mem[sizeof(T)];
        Status m_status = Status::Unuse;
    };

    // NOTE: why we don't use std::list? cause move constructor of std::list is
    // not noexcept, I don't want exception slow my performance
    template <typename U>
    struct Node {
        U m_data;
        Node* m_next{};
    };

    struct Block {
        const size_t m_block_mem_count;
        Mem* m_mem{};
        Node<size_t>* m_unused_index_head{};
        Node<size_t>* m_inuse_index_head{};
        Node<size_t>* m_pending_delete_head{};

        Block(size_t block_mem_count)
            : m_block_mem_count{block_mem_count} {
            m_mem = new(std::nothrow) Mem[block_mem_count];
            if (!m_mem) {
                LOGE("allocate memory block failed");
                return;
            }

            Node<size_t>* prev_index = m_unused_index_head;
            for (size_t i = 0; i < m_block_mem_count; i++) {
                Node<size_t>* new_index = new(std::nothrow) Node<
                    size_t>;

                if (!new_index) {
                    LOGE("bad alloc exception! out of memory");
                    continue;
                }

                new_index->m_data = i;

                if (prev_index) {
                    prev_index->m_next = new_index;
                } else {
                    m_unused_index_head = new_index;
                }
                prev_index = new_index;
            }
        }

        template <typename... Args>
        T* Allocate(Args&&... args) noexcept {
            Node<size_t>* index_node = m_unused_index_head;
            Mem* mem = m_mem + index_node->m_data;

            NICKEL_ASSERT(mem->m_status == Mem::Status::Unuse);

            T* elem;
            try {
                elem = std::construct_at((T*)&mem->m_mem,
                                         std::forward<Args>(args)...);
                mem->m_status = Mem::Status::InUse;
            } catch (const std::exception& e){
                LOGE("catch exception when construct object: {}", e.what());
                return nullptr;
            }

            m_unused_index_head = m_unused_index_head->m_next;
            index_node->m_next = m_inuse_index_head;
            m_inuse_index_head = index_node;

            return elem;
        }

        void Deallocate(T* p) noexcept {
            NICKEL_ASSERT((std::ptrdiff_t(p) - std::ptrdiff_t(m_mem)) %
                          sizeof(Mem) == 0, "invalid memory address");
        
            size_t idx = (Mem*)p - m_mem;
            Mem* mem = m_mem + idx;
        
            if (mem->m_status != Mem::Status::InUse) {
                LOGE("memory is not in use when deallocate");
                return;
            }

            try {
                ((T*)&mem->m_mem)->~T();
            } catch(const std::exception& e) {
                LOGE("catch exception when destruct object {}", e.what());
            }
                    
            mem->m_status = Mem::Status::Unuse;

            Node<size_t>* in_use_node = m_inuse_index_head;
            m_inuse_index_head = in_use_node->m_next;
            in_use_node->m_data = idx;

            in_use_node->m_next = m_unused_index_head;
            m_unused_index_head = in_use_node;
        }

        void MarkAsGarbage(T* p) noexcept {
            NICKEL_ASSERT((std::ptrdiff_t(p) - std::ptrdiff_t(m_mem)) %
                          sizeof(Mem) == 0, "invalid memory address");

            size_t idx = (Mem*)p - m_mem;
            Mem* mem = m_mem + idx;

            if (mem->m_status != Mem::Status::InUse) {
                LOGE("memory is not in use when mark as garbage");
                return;
            }
            
            mem->m_status = Mem::Status::PendingDelete;

            Node<size_t>* in_use_node = m_inuse_index_head;
            m_inuse_index_head = in_use_node->m_next;
            in_use_node->m_data = idx;

            in_use_node->m_next = m_pending_delete_head;
            m_pending_delete_head = in_use_node;
        }

        T* RequireReuse() noexcept {
            if (!m_pending_delete_head) {
                return nullptr;
            }

            Node<size_t>* node = m_pending_delete_head;
            m_pending_delete_head = m_pending_delete_head->m_next;

            size_t idx = node->m_data;
            Mem* mem = m_mem + idx;

            if (mem->m_status != Mem::Status::PendingDelete) {
                LOGE("reuse an in-use memory!");
                return nullptr;
            }

            mem->m_status = Mem::Status::InUse;
            node->m_next = m_inuse_index_head;
            m_inuse_index_head = node;
            return (T*)&mem->m_mem;
        }

        size_t GC(size_t count) noexcept {
            if (!m_pending_delete_head) {
                return 0;
            }

            size_t delete_count = 0;
            Node<size_t>* node = m_pending_delete_head;
            Node<size_t>* prev;
            while (node && delete_count < count) {
                prev = node;
                delete_count++;
                node = node->m_next;

                Mem* mem = m_mem + prev->m_data;

                try {
                    ((T*)&mem->m_mem)->~T();
                } catch (const std::exception& e) {
                    LOGE("catch exception when destruct object {}", e.what());
                }
                
                mem->m_status = Mem::Status::Unuse;
            }

            Node<size_t>* pending_delete_head = m_pending_delete_head;
            m_pending_delete_head = prev->m_next;
            prev->m_next = m_unused_index_head;
            m_unused_index_head = pending_delete_head;

            return delete_count;
        }

        ~Block() noexcept {
            for (size_t i = 0; i < m_block_mem_count; i++) {
                Mem* mem = m_mem + i;
                if (mem->m_status != Mem::Status::Unuse) {
                    try {
                        ((T*)mem)->~T();
                    } catch (...) {
                        LOGE("catched exception when destruct object");
                    }
                }
            }

            freeNode(m_inuse_index_head);
            freeNode(m_unused_index_head);
            freeNode(m_pending_delete_head);

            delete[] m_mem;
        }

    private:
        void freeNode(Node<size_t>* node) noexcept {
            while (node) {
                Node<size_t>* cur = node;
                node = node->m_next;
                try {
                    delete cur;
                } catch (...) {
                    LOGE("catched exception when destruct object");
                }
            }
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
            Node<Block>* new_block = new(std::nothrow) Node<Block>{
                m_block_mem_count};
            if (!new_block) {
                return nullptr;
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
} // namespace nickel
