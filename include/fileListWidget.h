#ifndef WTEXTENSIONS_FILELISTWIDGET_H
#define WTEXTENSIONS_FILELISTWIDGET_H

// Standard C++ library

#include <filesystem>

// Wt Library

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WIconPair.h>

/* Implements a list that displays three columns. The first column is the file name. (Not the full path), the
 * second column displays the type of file. The last column displays an icon that allows 'deletion' of the file from the list.
 * This is implemented as a container with a MVC table view holding the information.
 * The icon needs to be supplied and be part of the resource pack.
 * A signal is emitted when the delete button is pressed.
 */

class CFileListWidget : public Wt::WContainerWidget
{
  public:
    CFileListWidget(Wt::WApplication &, Wt::WIconPair);
    virtual ~CFileListWidget() = default;

    void insert(std::filesystem::path, std::string);
    void insert(std::filesystem::path &&, std::string &&);

    void erase(std::filesystem::path const &);


  private:
    CFileListWidget() = delete;
    CFileListWidget(CFileListWidget const &) = delete;
    CFileListWidget(CFileListWidget &&) = delete;
    CFileListWidget &operator=(CFileListWidget const &) = delete;
    CFileListWidget &operator=(CFileListWidget &&) = delete;

    std::map<std::filesystem::path, std::string> fileTypes;
    Wt::WApplication &application;
    Wt::WIconPair iconPair;
}

#endif // WTEXTENSIONS_FILELISTWIDGET_H
