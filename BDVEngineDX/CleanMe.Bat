rmdir /S /Q Debug
rmdir /S /Q Release
del /F /S *.sdf
del /F /S /A:H *.suo

rmdir /S /Q BDVEngineDX\Debug
rmdir /S /Q BDVEngineDX\Release

rmdir /S /Q HardwareTransformDemo\Debug
rmdir /S /Q HardwareTransformDemo\Release

rmdir /S /Q AnimationBlendingDemo\Debug
rmdir /S /Q AnimationBlendingDemo\Release

rmdir /S /Q AnimationCompressionDemo\Debug
rmdir /S /Q AnimationCompressionDemo\Release

rmdir /S /Q NormalMappingDemo\Debug
rmdir /S /Q NormalMappingDemo\Release

rmdir /S /Q Libs\Debug
rmdir /S /Q Libs\Release