#include "stdafx.h"
#include "UIRadioButton.h"

namespace DuiLib
{
    CRadioButtonUI::CRadioButtonUI()
    {

    }

    CRadioButtonUI::~CRadioButtonUI()
    {
        if (!m_sGroupName.IsEmpty() && m_pManager) 
        {
            m_pManager->RemoveOptionGroup(m_sGroupName, this);
        }
    }

    LPCTSTR CRadioButtonUI::GetClass() const
    {
        return _T("RadioButtonUI");
    }

    LPVOID CRadioButtonUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_RADIOBUTTON) == 0) 
        {
            return static_cast<CRadioButtonUI*>(this);
        }

        return CCheckBoxUI::GetInterface(pstrName);
    }

    void CRadioButtonUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
    {
        CControlUI::SetManager(pManager, pParent, bInit);
        if (bInit && !m_sGroupName.IsEmpty()) 
        {
            if (m_pManager) 
            {
                m_pManager->AddOptionGroup(m_sGroupName, this);
            }
        }
    }

    LPCTSTR CRadioButtonUI::GetGroup() const
    {
        return m_sGroupName;
    }

    void CRadioButtonUI::SetGroup(LPCTSTR pStrGroupName)
    {
        if (pStrGroupName == NULL) 
        {
            if (m_sGroupName.IsEmpty()) 
            {
                return;
            }
            m_sGroupName.Empty();
        }
        else 
        {
            if (m_sGroupName == pStrGroupName) 
            {
                return;
            }

            if (!m_sGroupName.IsEmpty() && m_pManager)
            {
                m_pManager->RemoveOptionGroup(m_sGroupName, this);
            }

            m_sGroupName = pStrGroupName;
        }

        if (!m_sGroupName.IsEmpty()) 
        {
            if (m_pManager) 
            {
                m_pManager->AddOptionGroup(m_sGroupName, this);
            }
        }
        else 
        {
            if (m_pManager) 
            {
                m_pManager->RemoveOptionGroup(m_sGroupName, this);
            }
        }

        SetChecked(m_bChecked);
    }
    

    void CRadioButtonUI::SetChecked(bool bChecked)
    {
        if (m_bChecked == bChecked) 
        {
            return;
        }

        m_bChecked = bChecked;
        if (m_bChecked) 
        {
            m_uButtonState |= UISTATE_SELECTED;
        }
        else 
        {
            m_uButtonState &= ~UISTATE_SELECTED;
        }

        if (m_pManager != NULL) 
        {
            if (!m_sGroupName.IsEmpty()) 
            {
                if (m_bChecked) 
                {
                    CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
                    for (int i = 0; i < aOptionGroup->GetSize(); i++) 
                    {
                        CRadioButtonUI* pControl = static_cast<CRadioButtonUI*>(aOptionGroup->GetAt(i));
                        if (pControl != this) 
                        {
                            pControl->SetChecked(false);
                        }
                    }

                    m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
                }
            }
            else 
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
            }
        }

        Invalidate();
    }

    bool CRadioButtonUI::Activate()
    {
        if (!CCheckBoxUI::Activate()) 
        {
            return false;
        }

        if (!m_sGroupName.IsEmpty()) 
        {
            SetChecked(true);
        }
        else 
        {
            SetChecked(!m_bChecked);
        }

        return true;
    }

    void CRadioButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (lstrcmpi(pstrName, _T("group")) == 0) 
        {
            SetGroup(pstrValue);
        }
        else 
        {
            CCheckBoxUI::SetAttribute(pstrName, pstrValue);
        }
    }
}
