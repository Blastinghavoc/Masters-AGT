project "assimp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("inter/" .. outputdir .. "/%{prj.name}")

    warnings "Off"

    defines 
    {
    -- For more info on the formats: http://assimp.sourceforge.net/main_features_formats.html
    -- Also check ImporterRegistry.cpp for all the macro guards
    -- Uncomment to build importers
    -- Comment to stop building them
    "ASSIMP_BUILD_NO_X_IMPORTER", -- DirectX X
    "ASSIMP_BUILD_NO_AMF_IMPORTER",
    -- "ASSIMP_BUILD_NO_3DS_IMPORTER", -- 3ds Max 3DS
    -- "ASSIMP_BUILD_NO_MDL_IMPORTER", -- Quake I
    -- "ASSIMP_BUILD_NO_MD2_IMPORTER", -- Quake II
    -- "ASSIMP_BUILD_NO_MD3_IMPORTER", -- Quake III
    "ASSIMP_BUILD_NO_PLY_IMPORTER", -- Stanford Polygon Library 
    "ASSIMP_BUILD_NO_ASE_IMPORTER", -- 3ds Max ASE ( .ase ) 
    -- "ASSIMP_BUILD_NO_OBJ_IMPORTER", -- obj
    "ASSIMP_BUILD_NO_HMP_IMPORTER", -- 3D GameStudio (3DGS) Terrain
    "ASSIMP_BUILD_NO_SMD_IMPORTER", -- Valve Model ( .smd,.vta )
    "ASSIMP_BUILD_NO_MDC_IMPORTER", -- Return to Castle Wolfenstein ( .mdc )
    -- "ASSIMP_BUILD_NO_MD5_IMPORTER", -- Doom 3 ( .md5* )
    "ASSIMP_BUILD_NO_STL_IMPORTER", -- Stereolithography 
    "ASSIMP_BUILD_NO_LWO_IMPORTER", -- LightWave  
    "ASSIMP_BUILD_NO_DXF_IMPORTER", -- AutoCad DXF
    "ASSIMP_BUILD_NO_NFF_IMPORTER", -- Neutral File Format / Sense8 WorldToolKit
    "ASSIMP_BUILD_NO_RAW_IMPORTER", -- PovRAY Raw 
    "ASSIMP_BUILD_NO_SIB_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_OFF_IMPORTER", -- Object File Format
    "ASSIMP_BUILD_NO_AC_IMPORTER",  -- AC3D
    "ASSIMP_BUILD_NO_BVH_IMPORTER", -- Biovision
    "ASSIMP_BUILD_NO_IRRMESH_IMPORTER", -- Irrlicht Mesh
    "ASSIMP_BUILD_NO_IRR_IMPORTER", -- Irrlicht Scene
    "ASSIMP_BUILD_NO_Q3D_IMPORTER", -- Quick3D
    "ASSIMP_BUILD_NO_B3D_IMPORTER", -- BlitzBasic 3D
    -- "ASSIMP_BUILD_NO_COLLADA_IMPORTER", -- Collada ( .dae )
    "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER", -- Terragen Terrain ( .ter )
    "ASSIMP_BUILD_NO_CSM_IMPORTER", -- CharacterStudio Motion
    "ASSIMP_BUILD_NO_3D_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_LWS_IMPORTER", -- LightWave Scene
    "ASSIMP_BUILD_NO_OGRE_IMPORTER", -- Ogre XML ( .xml )
    "ASSIMP_BUILD_NO_OPENGEX_IMPORTER", -- Open Game Engine Exchange ( .ogex )
    "ASSIMP_BUILD_NO_MS3D_IMPORTER", -- Milkshape 3D ( .ms3d )
    "ASSIMP_BUILD_NO_COB_IMPORTER", -- TrueSpace
    "ASSIMP_BUILD_NO_BLEND_IMPORTER", -- Blender 3D ( .blend )
    "ASSIMP_BUILD_NO_Q3BSP_IMPORTER", -- Quake III Map/BSP ( .pk3 )
    "ASSIMP_BUILD_NO_NDO_IMPORTER", -- Izware Nendo ( .ndo )
    "ASSIMP_BUILD_NO_IFC_IMPORTER", -- Industry Foundation Classes (IFC/Step) ( .ifc )
    "ASSIMP_BUILD_NO_XGL_IMPORTER", -- XGL ( .xgl,.zgl )
    "ASSIMP_BUILD_NO_FBX_IMPORTER", -- Autodesk ( .fbx )
    "ASSIMP_BUILD_NO_ASSBIN_IMPORTER", -- 
    "ASSIMP_BUILD_NO_GLTF_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_C4D_IMPORTER", -- Cinema4D is MSVC only and needs some weird headers to work
    "ASSIMP_BUILD_NO_3MF_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_X3D_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_MMD_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_STEP_IMPORTER", -- ??
    "ASSIMP_BUILD_NO_EXPORT",
    "OPENDDL_STATIC_LIBARY",
    }

    files 
    {
    "code/**.cpp",
    "code/**.h",
    "contrib/irrXML/*.cpp",
    "contrib/irrXML/*.h",
    "contrib/unzip/*.c",
    "contrib/unzip/*.h",
    "contrib/openddlparser/code/*.cpp",
    "contrib/poly2tri/poly2tri/**.cc",
    "contrib/clipper/*.cpp",
    "contrib/zlib/*.c",
    "contrib/zlib/*.h"
    }

    includedirs 
    {
    "config/",
    "include/",
    "contrib/irrXML/",
    "contrib/rapidjson/include/",
    "contrib/openddlparser/include/",
    "contrib/unzip/",
    "contrib/zlib/",
    "./"
    }

    removefiles 
    {
    "code/Importer/IFC/IFCReaderGen_4.*",
    }

    filter "system:windows"
    systemversion "latest"
    cppdialect "C++17"
    staticruntime "On"

    buildoptions { "/bigobj" }
    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        defines "PYRO_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "PYRO_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "PYRO_DIST"
        runtime "Release"
        optimize "on"