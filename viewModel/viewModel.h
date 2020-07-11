#include"../common/cslbase.h"
#include"../model/model.h"
#include<memory>
class ViewModel:public PropertyTrigger
{
public:
    ViewModel(/* args */);
    ~ViewModel();

//properties
    std::shared_ptr<std::string> get_Htmltxt() noexcept;
    std::shared_ptr<std::string> get_Url() noexcept;
    std::shared_ptr<std::string> get_Dir() noexcept;

//commands
    CommandFunc get_SetUrlCommand();
    CommandFunc get_SetDirCommand();
//methods
    void SetModel(const std::shared_ptr<Model>& spModel);

    PropertyNotification get_notification();
private:


private:
    std::shared_ptr<Model>  m_spModel;
};
