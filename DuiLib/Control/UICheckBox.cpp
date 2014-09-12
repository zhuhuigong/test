#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
    CCheckBoxUI::CCheckBoxUI()
        : m_bChecked(false)
    {

    }

    CCheckBoxUI::~CCheckBoxUI()
    {

    }

    LPCTSTR CCheckBoxUI::GetClass() const
    {
        return _T("CheckBoxUI");
    }

    LPVOID CCheckBoxUI::GetInterface(LPCTSTR pstrName)
    {
        if (lstrcmpi(pstrName, DUI_CTR_CHECKBOX) == 0)
        {
            return static_cast<CCheckBoxUI*>(this);
        }

        return CButtonUI::GetInterface(pstrName);
    }

    bool CCheckBoxUI::IsChecked() const
    {
        return m_bChecked;
    }

    void CCheckBoxUI::SetChecked(bool bChecked)
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
            m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
        }

        Invalidate();
    }

    bool CCheckBoxUI::Activate()
    {
        if (!CButtonUI::Activate())
        {
            return false;
        }

        SetChecked(!m_bChecked);
        return true;
    }

    void CCheckBoxUI::SetEnabled(bool bEnable)
    {
        CControlUI::SetEnabled(bEnable);

        if (!IsEnabled())
        {
            m_uButtonState = m_bChecked ? UISTATE_SELECTED : 0;
        }
    }

    LPCTSTR CCheckBoxUI::GetCheckedNormalImage()
    {
        return m_sCheckedNormalImage;
    }

    void CCheckBoxUI::SetCheckedNormalImage(LPCTSTR pStrImage)
    {
        m_sCheckedNormalImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CCheckBoxUI::GetCheckedHotImage()
    {
        return m_sCheckedHotImage;
    }

    void CCheckBoxUI::SetCheckedHotImage(LPCTSTR pStrImage)
    {
        m_sCheckedHotImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CCheckBoxUI::GetCheckedPushedImage()
    {
        return m_sCheckedPushedImage;
    }

    void CCheckBoxUI::SetCheckedPushedImage(LPCTSTR pStrImage)
    {
        m_sCheckedPushedImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CCheckBoxUI::GetCheckedFocusedImage()
    {
        return m_sCheckedFocusedImage;
    }

    void CCheckBoxUI::SetCheckedFocusedImage(LPCTSTR pStrImage)
    {
        m_sCheckedFocusedImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CCheckBoxUI::GetCheckedDisabledImage()
    {
        return m_sCheckedDisabledImage;
    }

    void CCheckBoxUI::SetCheckedDisabledImage(LPCTSTR pStrImage)
    {
        m_sCheckedDisabledImage = pStrImage;
        Invalidate();
    }

    SIZE CCheckBoxUI::EstimateSize(SIZE szAvailable)
    {
        if (m_cxyFixed.cy == 0)
        {
            return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
        }

        return CControlUI::EstimateSize(szAvailable);
    }

    void CCheckBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (lstrcmpi(pstrName, _T("checked")) == 0)
        {
            SetChecked(lstrcmpi(pstrValue, _T("true")) == 0);
        }
        else if (lstrcmpi(pstrName, _T("checkednormalimage")) == 0)
        {
            SetCheckedNormalImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("checkedhotimage")) == 0)
        {
            SetCheckedHotImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("checkedpushedimage")) == 0)
        {
            SetCheckedPushedImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("checkedfocusedimage")) == 0)
        {
            SetCheckedFocusedImage(pstrValue);
        }
        else if (lstrcmpi(pstrName, _T("checkeddisabledimage")) == 0)
        {
            SetCheckedDisabledImage(pstrValue);
        }
        /*else if(lstrcmpi(pstrName, _T("selectedbkcolor")) == 0 ) {
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetCheckedBkColor(clrColor);
            }
            else if(lstrcmpi(pstrName, _T("selectedtextcolor")) == 0 ) {
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetCheckedTextColor(clrColor);
            }*/
        else
        {
            CButtonUI::SetAttribute(pstrName, pstrValue);
        }
    }

    void CCheckBoxUI::PaintStatusImage(HDC hDC)
    {
        if (IsFocused())
        {
            m_uButtonState |= UISTATE_FOCUSED;
        }
        else
        {
            m_uButtonState &= ~UISTATE_FOCUSED;
        }

        if (!IsEnabled())
        {
            m_uButtonState |= UISTATE_DISABLED;
        }
        else
        {
            m_uButtonState &= ~UISTATE_DISABLED;
        }

        if (IsChecked())
        {
            m_uButtonState |= UISTATE_SELECTED;
        }
        else
        {
            m_uButtonState &= ~UISTATE_SELECTED;
        }

        // 未选中时绘制状态图片由父类CButtonUI完成
        if (!IsChecked())
        {
            CButtonUI::PaintStatusImage(hDC);
        }
        else
        {
            do
            {
                // 绘制状态图片时，各种状态之间是互斥的，绘制了一种状态图，就不能再绘制另一种状态图
                // 否则会出现状态图重叠现象，此时的效果就不是你想要的了，避免用goto这里用了do..while
                if ((m_uButtonState & UISTATE_DISABLED) != 0)
                {
                    if (!m_sCheckedDisabledImage.IsEmpty())
                    {
                        if (!DrawImage(hDC, (LPCTSTR)m_sCheckedDisabledImage))
                        {
                            m_sCheckedDisabledImage.Empty();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if ((m_uButtonState & UISTATE_PUSHED) != 0)
                {
                    if (!m_sCheckedPushedImage.IsEmpty())
                    {
                        if (!DrawImage(hDC, (LPCTSTR)m_sCheckedPushedImage))
                        {
                            m_sCheckedPushedImage.Empty();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if ((m_uButtonState & UISTATE_HOT) != 0)
                {
                    if (!m_sCheckedHotImage.IsEmpty())
                    {
                        if (!DrawImage(hDC, (LPCTSTR)m_sCheckedHotImage))
                        {
                            m_sCheckedHotImage.Empty();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
                {
                    if (!m_sCheckedFocusedImage.IsEmpty())
                    {
                        if (!DrawImage(hDC, (LPCTSTR)m_sCheckedFocusedImage))
                        {
                            m_sCheckedFocusedImage.Empty();
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                // 都不是以上状态，那么就是正常状态或使用前景图片
                if (!m_sCheckedNormalImage.IsEmpty())
                {
                    if (!DrawImage(hDC, (LPCTSTR)m_sCheckedNormalImage))
                    {
                        m_sCheckedNormalImage.Empty();
                    }
                    else
                    {
                        break;
                    }
                }

            } while (false);

            // 注意顺序，前景图放到最后才绘制，不然怎么叫“前景”图呢？
            if (!m_sForeImage.IsEmpty())
            {
                if (!DrawImage(hDC, (LPCTSTR)m_sForeImage))
                    m_sForeImage.Empty();
            }
        }
    }

}
