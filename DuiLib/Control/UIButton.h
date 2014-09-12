﻿#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib
{
    class UILIB_API CButtonUI : public CLabelUI
    {
    public:
        CButtonUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;

        bool Activate();
        void SetEnabled(bool bEnable = true);
        void DoEvent(TEventUI& event);
        void SetCursor(LPCTSTR pStrCursor, bool bModify);

        LPCTSTR GetNormalImage();
        void SetNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetHotImage();
        void SetHotImage(LPCTSTR pStrImage);
        LPCTSTR GetPushedImage();
        void SetPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetFocusedImage();
        void SetFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetDisabledImage();
        void SetDisabledImage(LPCTSTR pStrImage);
        LPCTSTR GetForeImage();
        void SetForeImage(LPCTSTR pStrImage);
        LPCTSTR GetHotForeImage();
        void SetHotForeImage(LPCTSTR pStrImage);

        void SetHotBkColor(DWORD dwColor);
        DWORD GetHotBkColor() const;
        void SetPushedBkColor(DWORD dwColor);
        DWORD GetPushedBkColor() const;
        void SetFocusedBkColor(DWORD dwColor);
        DWORD GetFocusedBkColor() const;
        void SetDisabledBkColor(DWORD dwColor);
        DWORD GetDisabledBkColor() const;

        void SetHotTextColor(DWORD dwColor);
        DWORD GetHotTextColor() const;
        void SetPushedTextColor(DWORD dwColor);
        DWORD GetPushedTextColor() const;
        void SetFocusedTextColor(DWORD dwColor);
        DWORD GetFocusedTextColor() const;

        SIZE EstimateSize(SIZE szAvailable);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintBkColor(HDC hDC);
        void PaintText(HDC hDC);
        void PaintStatusImage(HDC hDC);

    protected:
        UINT m_uButtonState;

        DWORD m_dwHotBkColor;
        DWORD m_dwPushedBkColor;
        DWORD m_dwFocusedBkColor;
        DWORD m_dwDisabledBkColor;

        DWORD m_dwHotTextColor;
        DWORD m_dwPushedTextColor;
        DWORD m_dwFocusedTextColor;

        CDuiString m_sNormalImage;
        CDuiString m_sHotImage;
        CDuiString m_sHotForeImage;
        CDuiString m_sPushedImage;
        CDuiString m_sPushedForeImage;
        CDuiString m_sFocusedImage;
        CDuiString m_sDisabledImage;
    };

}   // namespace DuiLib

#endif // __UIBUTTON_H__