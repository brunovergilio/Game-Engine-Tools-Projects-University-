rmdir /S /Q .vs
rmdir /S /Q Debug
rmdir /S /Q Release
del /F /S *.sdf
del /F /S *.db
del /F /S *.opendb
del /F /S /A:H *.suo

rmdir /S /Q FBXExporter\Debug
