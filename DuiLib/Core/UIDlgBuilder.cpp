#include "StdAfx.h"

namespace DuiLib {

    CDialogBuilder::CDialogBuilder()
        : m_pCallback(NULL)
        , m_pstrtype(NULL)
    {

    }

    CControlUI* CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback* pCallback,
        CPaintManagerUI* pManager, CControlUI* pParent)
    {
        //资源ID为0-65535，两个字节；字符串指针为4个字节
        //字符串以<开头认为是XML字符串，否则认为是XML文件
        if (HIWORD(xml.m_lpstr) != NULL)
        {
            if (xml.m_lpstr[0] == _T('<'))
            {
                // 以xml字符串方式加载
                if (!m_xml.Load(xml.m_lpstr))
                {
                    return NULL;
                }
            }
            else
            {
                // 以xml文件方式加载
                if (!m_xml.LoadFromFile(xml.m_lpstr))
                {
                    return NULL;
                }
            }
        }
        else
        {
            // 高16位为0，则认为是资源ID，从资源中读取xml
            HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
            if (hResource == NULL)
                return NULL;

            HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
            if (hGlobal == NULL)
            {
                FreeResource(hResource);
                return NULL;
            }

            m_pCallback = pCallback;
            if (!m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource)))
                return NULL;

            ::FreeResource(hResource);
            m_pstrtype = type;
        }

        // 上面的Load或其它加载函数已经加载、解析xml到对象中，现在
        // 开始具体分析每个节点及属性值，并new出控件类，设置类的成员
        return Create(pCallback, pManager, pParent);
    }

    CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
    {
        m_pCallback = pCallback;
        CMarkupNode root = m_xml.GetRoot();
        if (!root.IsValid())
            return NULL;

        // 这个pManager就是duilib窗口的CPaintManagerUI，一般是从OnCreate那里传进来
        if (pManager == NULL)
            return _Parse(&root, pParent, pManager);

        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;

        // 解析root节点的子节点，一般有Image、Font、Default，root即Window节点
        for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling())
        {
            pstrClass = node.GetName();
            if (lstrcmpi(pstrClass, _T("Image")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pImageName = NULL;
                LPCTSTR pImageResType = NULL;
                DWORD mask = 0;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if (lstrcmpi(pstrName, _T("name")) == 0)
                    {
                        pImageName = pstrValue;
                    }
                    else if (lstrcmpi(pstrName, _T("restype")) == 0)
                    {
                        pImageResType = pstrValue;
                    }
                    else if (lstrcmpi(pstrName, _T("mask")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        mask = _tcstoul(pstrValue, &pstr, 16);
                    }
                }

                if (pImageName != NULL)
                    pManager->AddImage(pImageName, pImageResType, mask);
            }
            else if (lstrcmpi(pstrClass, _T("Font")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pFontName = NULL;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (lstrcmpi(pstrName, _T("name")) == 0)
                    {
                        pFontName = pstrValue;
                    }
                    else if (lstrcmpi(pstrName, _T("size")) == 0)
                    {
                        size = _tcstol(pstrValue, &pstr, 10);
                    }
                    else if (lstrcmpi(pstrName, _T("bold")) == 0)
                    {
                        bold = (lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                    else if (lstrcmpi(pstrName, _T("underline")) == 0)
                    {
                        underline = (lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                    else if (lstrcmpi(pstrName, _T("italic")) == 0)
                    {
                        italic = (lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                    else if (lstrcmpi(pstrName, _T("default")) == 0)
                    {
                        defaultfont = (lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                }

                if (pFontName != NULL)
                {
                    pManager->AddFont(pFontName, size, bold, underline, italic);
                    if (defaultfont)
                        pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
                }
            }
            else if (lstrcmpi(pstrClass, _T("Default")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pControlName = NULL;
                LPCTSTR pControlValue = NULL;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (lstrcmpi(pstrName, _T("name")) == 0)
                    {
                        pControlName = pstrValue;
                    }
                    else if (lstrcmpi(pstrName, _T("value")) == 0)
                    {
                        pControlValue = pstrValue;
                    }
                }

                if (pControlName != NULL)
                {
                    pManager->AddDefaultAttributeList(pControlName, pControlValue);
                }
            }
        }

        // 解析Window节点！
        pstrClass = root.GetName();
        if (lstrcmpi(pstrClass, _T("Window")) == 0)
        {
            if (pManager->GetPaintWindow())
            {
                int nAttributes = root.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    if (lstrcmpi(pstrName, _T("size")) == 0)
                    {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        pManager->SetInitSize(cx, cy);
                    }
                    else if (lstrcmpi(pstrName, _T("sizebox")) == 0)
                    {
                        RECT rcSizeBox = { 0 };
                        LPTSTR pstr = NULL;
                        rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                        pManager->SetSizeBox(rcSizeBox);
                    }
                    else if (lstrcmpi(pstrName, _T("caption")) == 0)
                    {
                        RECT rcCaption = { 0 };
                        LPTSTR pstr = NULL;
                        rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                        pManager->SetCaptionRect(rcCaption);
                    }
                    else if (lstrcmpi(pstrName, _T("roundcorner")) == 0)
                    {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        pManager->SetRoundCorner(cx, cy);
                    }
                    else if (lstrcmpi(pstrName, _T("mininfo")) == 0)
                    {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        pManager->SetMinInfo(cx, cy);
                    }
                    else if (lstrcmpi(pstrName, _T("maxinfo")) == 0)
                    {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        pManager->SetMaxInfo(cx, cy);
                    }
                    else if (lstrcmpi(pstrName, _T("showdirty")) == 0)
                    {
                        pManager->SetShowUpdateRect(lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                    else if (lstrcmpi(pstrName, _T("alpha")) == 0)
                    {
                        pManager->SetTransparent(_ttoi(pstrValue));
                    }
                    else if (lstrcmpi(pstrName, _T("bktrans")) == 0)
                    {
                        pManager->SetBackgroundTransparent(lstrcmpi(pstrValue, _T("true")) == 0);
                    }
                    else if (lstrcmpi(pstrName, _T("disabledfontcolor")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultDisabledColor(clrColor);
                    }
                    else if (lstrcmpi(pstrName, _T("defaultfontcolor")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultFontColor(clrColor);
                    }
                    else if (lstrcmpi(pstrName, _T("linkfontcolor")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultLinkFontColor(clrColor);
                    }
                    else if (lstrcmpi(pstrName, _T("linkhoverfontcolor")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultLinkHoverFontColor(clrColor);
                    }
                    else if (lstrcmpi(pstrName, _T("selectedcolor")) == 0)
                    {
                        if (*pstrValue == _T('#'))
                            pstrValue = ::CharNext(pstrValue);

                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultSelectedBkColor(clrColor);
                    }
                }
            }
        }
        
        // 解析Window节点的其它子节点（因上面代码已处理Image、Font、Default节点，_Parse中会跳过这些）
        return _Parse(&root, pParent, pManager);
    }

    CMarkup* CDialogBuilder::GetMarkup()
    {
        return &m_xml;
    }

    void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
    {
        return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
    }

    void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
    {
        return m_xml.GetLastErrorLocation(pstrSource, cchMax);
    }

    CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
    {
        IContainerUI* pContainer = NULL;
        CControlUI* pReturn = NULL;

        // 这里解析的是Window节点的子节点，你可以有多个容器子节点（Window节点下），当然通常只有一个
        for (CMarkupNode node = pRoot->GetChild(); node.IsValid(); node = node.GetSibling())
        {
            LPCTSTR pstrClass = node.GetName();

            // 如果节点是Image、Font、Default，则跳过，因为之前已经处理过了
            if (lstrcmpi(pstrClass, _T("Image")) == 0 ||
                lstrcmpi(pstrClass, _T("Font")) == 0 ||
                lstrcmpi(pstrClass, _T("Default")) == 0)
                continue;

            CControlUI* pControl = NULL;

            // 解析Include节点，这里之所以有continue，是因为Include不是控件节点，没有必要执行后面的代码
            if (lstrcmpi(pstrClass, _T("Include")) == 0)
            {
                // 如果Include没有属性（是空节点）则跳过，不用解析了
                if (!node.HasAttributes())
                    continue;

                int count = 1;
                LPTSTR pstr = NULL;
                TCHAR szValue[500] = { 0 };
                SIZE_T cchLen = lengthof(szValue) - 1;
                if (node.GetAttributeValue(_T("count"), szValue, cchLen))
                    count = _tcstol(szValue, &pstr, 10);

                cchLen = lengthof(szValue) - 1;
                if (!node.GetAttributeValue(_T("source"), szValue, cchLen))
                    continue;

                for (int i = 0; i < count; i++)
                {
                    CDialogBuilder builder;
                    if (m_pstrtype != NULL)
                    {
                        // 使用资源dll，从资源中读取
                        WORD id = (WORD)_tcstol(szValue, &pstr, 10);
                        pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
                    }
                    else
                    {
                        pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
                    }
                }
                continue;
            }
            else if (lstrcmpi(pstrClass, _T("TreeNode")) == 0)
            {
                // 树控件XML解析
                CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
                CTreeNodeUI* pNode = new CTreeNodeUI();
                if (pParentNode != NULL)
                {
                    if (!pParentNode->Add(pNode))
                    {
                        delete pNode;
                        continue;
                    }
                }

                // 若有控件默认配置先初始化默认属性
                if (pManager != NULL)
                {
                    // 将窗口的pManager指针传给控件的m_pManager成员
                    pNode->SetManager(pManager, NULL, false);

                    LPCTSTR pDefaultAttributes = NULL;
                
                    // 如果有名称为"*"的默认属性，则会应用到所有控件中！
                    if (pDefaultAttributes = pManager->GetDefaultAttributeList(_T("*")))
                    {
                        // 将属性列表应用到对应的控件类，这就是默认属性列表！下面会调用SetAttribute，一个改变的机会！
                        pNode->ApplyAttributeList(pDefaultAttributes);
                    }

                    // 如果控件有默认属性，则应用到这个控件（如果有*的，会覆盖上）
                    if (pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass))
                    {
                        // 将属性列表应用到对应的控件类，这就是默认属性列表！下面会调用SetAttribute，一个改变的机会！
                        pNode->ApplyAttributeList(pDefaultAttributes);
                    }
                }

                // 解析所有属性并覆盖默认属性
                if (node.HasAttributes())
                {
                    int nAttributes = node.GetAttributeCount();
                    for (int i = 0; i < nAttributes; i++)
                    {
                        pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
                    }
                }

                //检索子节点及附加控件
                if (node.HasChildren())
                {
                    CControlUI* pSubControl = _Parse(&node, pNode, pManager);
                    if (pSubControl && lstrcmpi(pSubControl->GetClass(), _T("TreeNodeUI")) != 0)
                    {
                        //pSubControl->SetFixedWidth(30);
                        //CHorizontalLayoutUI* pHorz = pNode->GetTreeNodeHoriznotal();
                        //pHorz->Add(new CEditUI());
                        //continue;
                    }
                }

                if (!pParentNode)
                {
                    CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));

                    ASSERT(pTreeView);
                    if (pTreeView == NULL)
                        return NULL;

                    if (!pTreeView->Add(pNode))
                    {
                        delete pNode;
                        pNode = NULL;
                        continue;
                    }
                }
                continue;
            }
            else
            {
                // 不是前面的，那么就是控件节点了，根据节点名new这些控件类，得到控件指针pControl
                // 这里先得到控制名称长度是有用的，用switch语句来控制，这样可以减少比较次数！！
                SIZE_T cchLen = lstrlen(pstrClass);
                switch (cchLen)
                {
                case 4:
                    if (lstrcmpi(pstrClass, DUI_CTR_EDIT) == 0)                   pControl = new CEditUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_LIST) == 0)              pControl = new CListUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_TEXT) == 0)              pControl = new CTextUI;
                    break;
                case 5:
                    if (lstrcmpi(pstrClass, DUI_CTR_COMBO) == 0)                  pControl = new CComboUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_LABEL) == 0)             pControl = new CLabelUI;
                    //else if( lstrcmpi(pstrClass, DUI_CTR_FLASH) == 0 )             pControl = new CFlashUI;
                    break;
                case 6:
                    if (lstrcmpi(pstrClass, DUI_CTR_BUTTON) == 0)                 pControl = new CButtonUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_OPTION) == 0)            pControl = new COptionUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_SLIDER) == 0)            pControl = new CSliderUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_SHADOW) == 0)            pControl = new CShadowUI;
                    break;
                case 7:
                    if (lstrcmpi(pstrClass, DUI_CTR_CONTROL) == 0)                pControl = new CControlUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_ACTIVEX) == 0)           pControl = new CActiveXUI;
                    break;
                case 8:
                    if (lstrcmpi(pstrClass, DUI_CTR_PROGRESS) == 0)               pControl = new CProgressUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_RICHEDIT) == 0)          pControl = new CRichEditUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_CHECKBOX) == 0)          pControl = new CCheckBoxUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_COMBOBOX) == 0)          pControl = new CComboBoxUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_DATETIME) == 0)          pControl = new CDateTimeUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_TREEVIEW) == 0)          pControl = new CTreeViewUI;
                    break;
                case 9:
                    if (lstrcmpi(pstrClass, DUI_CTR_CONTAINER) == 0)              pControl = new CContainerUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_TABLAYOUT) == 0)         pControl = new CTabLayoutUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_SCROLLBAR) == 0)         pControl = new CScrollBarUI;
                    break;
                case 10:
                    if (lstrcmpi(pstrClass, DUI_CTR_LISTHEADER) == 0)             pControl = new CListHeaderUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_TILELAYOUT) == 0)        pControl = new CTileLayoutUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_WEBBROWSER) == 0)        pControl = new CWebBrowserUI;
                    break;
                case 11:
                    if (lstrcmpi(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)            pControl = new CChildLayoutUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_RADIOBUTTON) == 0)       pControl = new CRadioButtonUI;
                    break;
                case 14:
                    if (lstrcmpi(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0)         pControl = new CVerticalLayoutUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_LISTHEADERITEM) == 0)    pControl = new CListHeaderItemUI;
                    break;
                case 15:
                    if (lstrcmpi(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0)        pControl = new CListTextElementUI;
                    break;
                case 16:
                    if (lstrcmpi(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0)       pControl = new CHorizontalLayoutUI;
                    else if (lstrcmpi(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0)  pControl = new CListLabelElementUI;
                    break;
                case 20:
                    if (lstrcmpi(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0)   pControl = new CListContainerElementUI;
                    break;
                }

                // 用户提供的控件工厂，就是说如果XML中没找到控件，则看看有没有插件DLL
                if (pControl == NULL)
                {
                    CStdPtrArray* pPlugins = CPaintManagerUI::GetPlugins();
                    LPCREATECONTROL lpCreateControl = NULL;
                    for (int i = 0; i < pPlugins->GetSize(); ++i)
                    {
                        lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
                        if (lpCreateControl != NULL)
                        {
                            pControl = lpCreateControl(pstrClass);
                            if (pControl != NULL)
                                break;
                        }
                    }
                }

                // 如果此控件为NULL，但是回调不为空，就用回调来new一个控件
                if (pControl == NULL && m_pCallback != NULL)
                {
                    pControl = m_pCallback->CreateControl(pstrClass);
                }
            }

#ifndef _DEBUG
            ASSERT(pControl);
#endif // _DEBUG
            if (pControl == NULL)
            {
#ifdef _DEBUG
                DUITRACE(_T("未知控件:%s"), pstrClass);
#else
                continue;
#endif
            }

            // 如果控件节点还有子控件（如容器控件），则递归调用_Parse解析它的子控件（节点）
            if (node.HasChildren())
            {
                _Parse(&node, pControl, pManager);
            }

            // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口，一般情况下pParent是NULL的
            if (pParent != NULL)
            {
                CTreeNodeUI* pContainerNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
                if (pContainerNode != NULL)
                {
                    pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
                }
                else
                {
                    if (pContainer == NULL)
                        pContainer = static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer")));

                    ASSERT(pContainer);
                    if (pContainer == NULL)
                        return NULL;

                    if (!pContainer->Add(pControl))
                    {
                        delete pControl;
                        pControl = NULL;
                        continue;
                    }
                }
            }

            // 初始化默认属性
            if (pManager != NULL)
            {
                // 将窗口的pManager指针传给控件的m_pManager成员
                pControl->SetManager(pManager, NULL, false);

                LPCTSTR pDefaultAttributes = NULL;
                
                // 如果有名称为"*"的默认属性，则会应用到所有控件中！
                if (pDefaultAttributes = pManager->GetDefaultAttributeList(_T("*")))
                {
                    // 将属性列表应用到对应的控件类，这就是默认属性列表！下面会调用SetAttribute，一个改变的机会！
                    pControl->ApplyAttributeList(pDefaultAttributes);
                }

                // 如果控件有默认属性，则应用到这个控件（如果有*的，会覆盖上）
                if (pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass))
                {
                    // 将属性列表应用到对应的控件类，这就是默认属性列表！下面会调用SetAttribute，一个改变的机会！
                    pControl->ApplyAttributeList(pDefaultAttributes);
                }
            }

            // 解析属性并应用
            if (node.HasAttributes())
            {
                int nAttributes = node.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++)
                {
                    // 调用控件类的SetAttribute虚函数设置/应用属性
                    pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
                }
            }

            if (pManager != NULL)
            {
                // 又将控件类的m_pManager成员置NULL
                pControl->SetManager(NULL, NULL, false);
            }

            // 返回第一项，当然通常只有一项啦，而这个通常是控件类
            if (pReturn == NULL)
                pReturn = pControl;
        }

        return pReturn;
    }

} // namespace DuiLib
