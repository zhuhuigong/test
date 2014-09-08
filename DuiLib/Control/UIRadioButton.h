#ifndef __UIRADIOBUTTON_H__
#define __UIRADIOBUTTON_H__

#pragma once

// 新增类，是真正的RadioButton，继承自CCheckBoxUI，为单选按钮

namespace DuiLib
{
    class UILIB_API CRadioButtonUI : public CCheckBoxUI
    {
    public:
        CRadioButtonUI();
        ~CRadioButtonUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

        bool Activate();
    
        LPCTSTR GetGroup() const;
        void SetGroup(LPCTSTR pStrGroupName = NULL);

        void SetChecked(bool bChecked);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);


    protected:
        CDuiString      m_sGroupName;
    };
}

#endif // __UIRADIOBUTTON_H__
