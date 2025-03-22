#include "nickel/fs/dialog.hpp"

#include "nickel/common/internal/sdl_call.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel {

class FileDialog::Impl {
public:
    enum class Type { OpenFile, SaveFile, OpenFolder };

    explicit Impl(Type type);
    ~Impl();
    void SetTitle(const std::string&);
    void SetAcceptButtonText(const std::string&);
    void SetCancelButtonText(const std::string&);
    void AddFilter(const std::string& name, const std::string& pattern);
    void AllowMultipleSelect(bool);
    void SetDefaultFolder(const std::string&);
    void Open();

    const std::vector<Path>& GetSelectedFiles() const;

private:
    struct Filter {
        std::string name;
        std::string pattern;
    };

    Type m_type;
    std::vector<Filter> m_filters;
    std::string m_default_folder;
    bool m_allow_multiple_select = false;
    std::string m_title_label;
    std::string m_accept_label;
    std::string m_cancel_label;
    SDL_PropertiesID m_properties;

    std::promise<std::vector<Path>> m_promise;
    std::vector<Path> m_results;

    static void callback(void* userdata, const char* const* filelist,
                         int filter);
};

FileDialog::Impl::Impl(Type type)
    : m_type{type}, m_properties{SDL_CreateProperties()} {}

FileDialog::Impl::~Impl() {
    SDL_DestroyProperties(m_properties);
}

void FileDialog::Impl::SetTitle(const std::string& title) {
    m_title_label = title;
}

void FileDialog::Impl::SetAcceptButtonText(const std::string& text) {
    m_accept_label = text;
}

void FileDialog::Impl::SetCancelButtonText(const std::string& text) {
    m_cancel_label = text;
}

void FileDialog::Impl::AddFilter(const std::string& name,
                                 const std::string& pattern) {
    m_filters.push_back({name, pattern});
}

void FileDialog::Impl::AllowMultipleSelect(bool allow) {
    m_allow_multiple_select = allow;
}

void FileDialog::Impl::SetDefaultFolder(const std::string& folder) {
    // TODO: check folder is absolute path
    m_default_folder = folder;
}

void FileDialog::Impl::Open() {
    std::future f = m_promise.get_future();

    SDL_FileDialogType dialog_type{};
    switch (m_type) {
        case Type::OpenFile:
            dialog_type = SDL_FILEDIALOG_OPENFILE;
            break;
        case Type::SaveFile:
            dialog_type = SDL_FILEDIALOG_SAVEFILE;
            break;
        case Type::OpenFolder:
            dialog_type = SDL_FILEDIALOG_OPENFOLDER;
            break;
    }

    std::vector<SDL_DialogFileFilter> filters;
    filters.reserve(m_filters.size());
    for (auto& filter : m_filters) {
        filters.push_back(
            SDL_DialogFileFilter{filter.name.c_str(), filter.pattern.c_str()});
    }

    SDL_CALL(SDL_SetStringProperty(m_properties,
                                   SDL_PROP_FILE_DIALOG_LOCATION_STRING,
                                   m_default_folder.c_str()));
    SDL_CALL(SDL_SetStringProperty(m_properties,
                                   SDL_PROP_FILE_DIALOG_TITLE_STRING,
                                   m_title_label.c_str()));
    SDL_CALL(SDL_SetStringProperty(m_properties,
                                   SDL_PROP_FILE_DIALOG_ACCEPT_STRING,
                                   m_accept_label.c_str()));
    SDL_CALL(SDL_SetStringProperty(m_properties,
                                   SDL_PROP_FILE_DIALOG_CANCEL_STRING,
                                   m_cancel_label.c_str()));
    SDL_CALL(SDL_SetBooleanProperty(m_properties,
                                    SDL_PROP_FILE_DIALOG_MANY_BOOLEAN,
                                    m_allow_multiple_select));
    SDL_CALL(SDL_SetNumberProperty(
        m_properties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, filters.size()));
    SDL_CALL(SDL_SetPointerProperty(
        m_properties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, filters.data()));

    SDL_ShowFileDialogWithProperties(dialog_type, &callback, this,
                                     m_properties);

    m_results = f.get();
}

const std::vector<Path>& FileDialog::Impl::GetSelectedFiles() const {
    return m_results;
}

void FileDialog::Impl::callback(void* userdata, const char* const* filelist,
                                int) {
    auto self = static_cast<Impl*>(userdata);

    std::vector<Path> results;
    while (filelist && *filelist) {
        const char* file = *filelist;
        results.push_back(file);
        filelist++;
    }

    self->m_promise.set_value(std::move(results));
}

FileDialog FileDialog::CreateOpenFileDialog() {
    return FileDialog{std::make_shared<Impl>(Impl::Type::OpenFile)};
}

FileDialog FileDialog::CreateSaveDialog() {
    return FileDialog{std::make_shared<Impl>(Impl::Type::SaveFile)};
}

FileDialog FileDialog::CreateOpenFolderDialog() {
    return FileDialog{std::make_shared<Impl>(Impl::Type::OpenFolder)};
}

FileDialog& FileDialog::SetTitle(const std::string& title) {
    m_impl->SetTitle(title);
    return *this;
}

FileDialog& FileDialog::SetAcceptButtonText(const std::string& text) {
    m_impl->SetAcceptButtonText(text);
    return *this;
}

FileDialog& FileDialog::SetCancelButtonText(const std::string& text) {
    m_impl->SetCancelButtonText(text);
    return *this;
}

FileDialog& FileDialog::AllowMultipleSelect(bool allow) {
    m_impl->AllowMultipleSelect(allow);
    return *this;
}

FileDialog& FileDialog::AddFilter(const std::string& name,
                                  const std::string& pattern) {
    m_impl->AddFilter(name, pattern);
    return *this;
}

FileDialog& FileDialog::SetDefaultFolder(const std::string& folder) {
    m_impl->SetDefaultFolder(folder);
    return *this;
}

FileDialog& FileDialog::Open() {
    m_impl->Open();
    return *this;
}

const std::vector<Path>& FileDialog::GetSelected() const {
    return m_impl->GetSelectedFiles();
}

FileDialog::FileDialog(std::shared_ptr<Impl> impl) : m_impl{impl} {}

}  // namespace nickel
