# פרויקט גמר — בינה מלאכותית למשחקים (ללא מצגת)
הפרויקט כולל גרפיקה (OpenGL/FreeGLUT) עם Fallback לקונסול (`USE_CONSOLE`). ראו הנחיות מפורטות ב-README_HE.md זה.
התקנת vcpkg חד־פעמית:
```
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd C:\vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install freeglut:x64-windows glew:x64-windows
```
לאחר מכן לפתוח `ai_battle.sln`, לבחור Debug|x64, וללחוץ F5.
