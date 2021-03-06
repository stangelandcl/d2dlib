namespace Direct2D {

public enum class WindowsMessages : uint32_t {
        WMNULL=0,
        WMCREATE,
        WMDESTROY,
        WMMOVE,
        WMSIZE=5,
        WMACTIVATE,
        WMSETFOCUS,
        WMKILLFOCUS,
        WMENABLE=0xA,
        WMSETTEXT=0xC,
        WMGETTEXT,
        WMGETTEXTLENGTH,
        WMPAINT,
        WMCLOSE,
        WMQUERYENDSESSION,
        WMQUERYOPEN=0x13,
        WMERASEBKGND,
        WMENDSESSION=0x16,
        WMSHOWWINDOW=0x18,
        WMCOPYDATA=0x4A,
        WMHSCROLL=0x114,
        WMVSCROLL=0x115,
        WMCONTEXTMENU=0x7B,
        WMKEYDOWN=0x100,
        WMKEYUP,
        WMMOUSEMOVE=0x200,
        WMCHAR=0x102,
        WMLBUTTONDOWN=0x201,
        WMLBUTTONUP,
        WMLBUTTONDBLCLK,
        WMRBUTTONDOWN,
        WMRBUTTONUP,
        WMRBUTTONDBLCLK,
        WMMOUSEWHEEL=0x20E,
        WMHOTKEY=0x312,
        WMDRAWCLIPBOARD=0x308,
        WMCHANGECBCHAIN=0x30D,
        WMUSER=0x400
};

}
