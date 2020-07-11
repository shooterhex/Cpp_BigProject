#include"viewModel.h"
#define HTMLTEXT 100
ViewModel::ViewModel()
{

};
ViewModel::~ViewModel(){};
void ViewModel::SetModel(const std::shared_ptr<Model>& spModel)
{
    m_spModel = spModel;
    m_spModel->AddNotification(get_notification());
};
//properties
std::shared_ptr<std::string> ViewModel::get_Htmltxt() noexcept
{
    return m_spModel->get_Htmltxt();
};
std::shared_ptr<std::string> ViewModel::get_Dir() noexcept
{
    return m_spModel->get_Dir();
};
std::shared_ptr<std::string> ViewModel::get_Url() noexcept
{
    return m_spModel->get_Url();
};
PropertyNotification ViewModel::get_notification()
{
    return [this](uint32_t uID)
            {
                if( uID == HTMLTEXT ) {
                    this->Fire(uID);
                }
            };
}
