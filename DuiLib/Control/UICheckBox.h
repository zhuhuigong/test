#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

// 有修改，完全重写了CCheckBoxUI类，继承自CButtonUI，和COptionUI平行
// 是真正的CheckBox类，支持10种状态图片！！为复选框

namespace DuiLib
{
    class UILIB_API CCheckBoxUI : public CButtonUI
    {
    public:
        CCheckBoxUI();
        ~CCheckBoxUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        bool Activate();
        void SetEnabled(bool bEnable = true);

        LPCTSTR GetCheckedNormalImage();
        void SetCheckedNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetCheckedHotImage();
        void SetCheckedHotImage(LPCTSTR pStrImage);
        LPCTSTR GetCheckedPushedImage();
        void SetCheckedPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetCheckedFocusedImage();
        void SetCheckedFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetCheckedDisabledImage();
        void SetCheckedDisabledImage(LPCTSTR pStrImage);

        bool IsChecked() const;
        virtual void SetChecked(bool bChecked);

        SIZE EstimateSize(SIZE szAvailable);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintStatusImage(HDC hDC);

    protected:
        bool            m_bChecked;
        //bool            m_bThreeState;

        CDuiString      m_sCheckedNormalImage;
        CDuiString      m_sCheckedHotImage;
        //CDuiString    m_sCheckedHotForeImage;
        CDuiString      m_sCheckedPushedImage;
        //CDuiString    m_sCheckedPushedForeImage;
        CDuiString      m_sCheckedFocusedImage;
        CDuiString      m_sCheckedDisabledImage;
    };
}

#endif // __UICHECKBOX_H__
