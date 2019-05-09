
#ifdef STRICT
#undef STRICT
#endif

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cstdint>
#include "Wincodec.h"

#include "Context.cpp"
#include "Bitmap.cpp"
#include "Geometry.cpp"
#include "Pen.cpp"
#include "Simple.cpp"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Text;
#using <System.Drawing.dll>

#include "dotnet/Enums.cpp"
#include "dotnet/Structs.cpp"
#include "dotnet/Classes.cpp"
