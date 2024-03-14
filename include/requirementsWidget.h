#ifndef WTEXTENSIONS_FILEMATCHWIDGET_H
#define WTEXTENSIONS_FILEMATCHWIDGET_H

/* The file match widget is used to display a two column table. Where the second column represents a file (resource) needed
 * by a WebApp, and the first column indicates whether the need is met.
 * The widget is positioned in a container. and can be placed within other widgets.
 * The widget is designed to be called from the main event thread, not another thread.
 */
 
 // Standard C++ library
 
 
 // Wt library
 #include <Wt/WApplication.h>
 
 template<typname T>
 class CFileMatchWidget : public Wt::WContainer
 {
   public:
    CFileMatchWidget(Wt::WApplication &);
    virtual ~CFileMatchWidget() = default;
    
    void insert(T, std::string &&);
    void insert(T, std::string const &);    
    void insert(std::list_initialiser<std::tuple<T, std::string> &);
    
    void changeStatus(T, bool = true);
    
   
   private:
    CFileMatchWidget() = delete;
    CFileMatchWidget(CFileMatchWidget const &) = delete;
    CFileMatchWidget(CFileMatchWidget &&) = delete;
    CFileMatchWidget &operator=(CFileMatchWidget const &) = delete;
    CFileMatchWidget &operator==(CFileMatchWidget &&) = delete;
    
    using record_t = std::pair<bool, std::string>;
    std::list<record_t> inOrder;  // Ordered copy of the data.
    std::map<T, std::reference_wraper<record_t>> byID;
 }


#endif // WTEXTENSIONS_FILEMATCHWIDGET_H