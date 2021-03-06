/*
 * Direct3D11 render system header
 * 
 * This file is part of the "SoftPixel Engine" (Copyright (c) 2008 by Lukas Hermanns)
 * See "SoftPixelEngine.hpp" for license information.
 */

#ifndef __SP_RENDERSYSTEM_DIRECT3D11_H__
#define __SP_RENDERSYSTEM_DIRECT3D11_H__


#include "Base/spStandard.hpp"

#if defined(SP_COMPILE_WITH_DIRECT3D11)


#include "RenderSystem/spRenderSystem.hpp"
#include "RenderSystem/Direct3D11/spDirect3D11Texture.hpp"
#include "RenderSystem/Direct3D11/spDirect3D11Shader.hpp"

#include <d3d11.h>
#include <d3dx11.h>


namespace sp
{
namespace video
{


//! Direct3D11 render system. This renderer supports Direct3D 11.0.
class SP_EXPORT Direct3D11RenderSystem : public RenderSystem
{
    
    public:
        
        Direct3D11RenderSystem();
        ~Direct3D11RenderSystem();
        
        /* === Initialization functions === */
        
        void setupConfiguration();
        
        /* === Renderer information === */
        
        io::stringc getRenderer()           const;
        io::stringc getVersion()            const;
        io::stringc getVendor()             const;
        io::stringc getShaderVersion()      const;
        
        s32 getMultitexCount()              const;
        s32 getMaxAnisotropicFilter()       const;
        s32 getMaxLightCount()              const;
        
        bool queryVideoSupport(const EVideoFeatureQueries Query) const;
        
        void printWarning();
        
        /* === Video buffer control functions === */
        
        void clearBuffers(const s32 ClearFlags = BUFFER_COLOR | BUFFER_DEPTH);
        
        /* === Configuration functions === */
        
        void setShadeMode(const EShadeModeTypes ShadeMode);
        
        void setClearColor(const color &Color);
        void setColorMask(bool isRed, bool isGreen, bool isBlue, bool isAlpha = true);
        void setDepthMask(bool isDepth);
        
        void setVsync(bool isVsync);
        
        void setAntiAlias(bool isAntiAlias);
        
        /* === Rendering functions === */
        
        void setupMaterialStates(const MaterialStates* Material);
        
        void setupShaderTable(const scene::MaterialNode* Object, ShaderTable* ShaderObject);
        
        void updateMaterialStates(MaterialStates* Material, bool isClear = false);
        
        void updateLight(
            u32 LightID, const scene::ELightModels LightType, bool isVolumetric,
            const dim::vector3df &Direction, f32 SpotInnerConeAngle, f32 SpotOuterConeAngle,
            f32 AttenuationConstant, f32 AttenuationLinear, f32 AttenuationQuadratic
        );
        
        /* === Hardware mesh buffers === */
        
        void createVertexBuffer(void* &BufferID);
        void createIndexBuffer(void* &BufferID);
        
        void deleteVertexBuffer(void* &BufferID);
        void deleteIndexBuffer(void* &BufferID);
        
        void updateVertexBuffer(
            void* BufferID, const dim::UniversalBuffer &BufferData, const VertexFormat* Format, const EMeshBufferUsage Usage
        );
        void updateIndexBuffer(
            void* BufferID, const dim::UniversalBuffer &BufferData, const IndexFormat* Format, const EMeshBufferUsage Usage
        );
        
        void updateVertexBufferElement(void* BufferID, const dim::UniversalBuffer &BufferData, u32 Index);
        void updateIndexBufferElement(void* BufferID, const dim::UniversalBuffer &BufferData, u32 Index);
        
        void drawMeshBuffer(const MeshBuffer* MeshBuffer);
        
        /* === Render states === */
        
        void setRenderState(const video::ERenderStates Type, s32 State);
        s32 getRenderState(const video::ERenderStates Type) const;
        
        void disable3DRenderStates();
        
        /* === Lighting === */
        
        void addDynamicLightSource(
            u32 LightID, scene::ELightModels Type,
            video::color &Diffuse, video::color &Ambient, video::color &Specular,
            f32 AttenuationConstant, f32 AttenuationLinear, f32 AttenuationQuadratic
        );
        
        void setLightStatus(u32 LightID, bool isEnable);
        void setLightColor(
            u32 LightID, const video::color &Diffuse, const video::color &Ambient, const video::color &Specular
        );
        
        /* === Fog effect === */
        
        void setFog(const EFogTypes Type);
        void setFogColor(const video::color &Color);
        void setFogRange(f32 Range, f32 NearPlane = 1.0f, f32 FarPlane = 1000.0f, const EFogModes Mode = FOG_PALE);
        
        /* === Clipping planes === */
        
        void setClipPlane(u32 Index, const dim::plane3df &Plane, bool Enable);
        
        /* === Shader programs === */
        
        ShaderTable* createShaderTable(VertexFormat* VertexInputLayout = 0);
        
        Shader* createShader(
            ShaderTable* ShaderTableObj, const EShaderTypes Type, const EShaderVersions Version,
            const std::vector<io::stringc> &ShaderBuffer, const io::stringc &EntryPoint = ""
        );
        
        void unbindShaders();
        
        bool runComputeShader(
            Shader* ShaderObj, ComputeShaderIO* IOInterface, const dim::vector3di &GroupSize
        );
        
        ComputeShaderIO* createComputeShaderIO();
        void deleteComputeShaderIO(ComputeShaderIO* &IOInterface);
        
        /* === Simple drawing functions === */
        
        void beginDrawing2D();
        void endDrawing2D();
        
        void beginDrawing3D();
        void endDrawing3D();
        
        void setBlending(const EBlendingTypes SourceBlend, const EBlendingTypes DestBlend);
        void setClipping(bool Enable, const dim::point2di &Position, const dim::size2di &Dimension);
        void setViewport(const dim::point2di &Position, const dim::size2di &Dimension);
        
        bool setRenderTarget(Texture* Target);
        
        /* === Image drawing === */
        
        void draw2DImage(
            Texture* Tex, const dim::point2di &Position, const color &Color = color(255)
        );
        
        void draw2DImage(
            Texture* Tex, const dim::rect2di &Position,
            const dim::rect2df &Clipping = dim::rect2df(0.0f, 0.0f, 1.0f, 1.0f),
            const color &Color = color(255)
        );
        
        /* === Texture loading and creating === */
        
        Texture* loadTexture(ImageLoader* Loader);
        
        Texture* copyTexture(const Texture* Tex);
        
        Texture* createTexture(
            const dim::size2di &Dimension, const EPixelFormats Format, const u8* ImageBuffer, const io::stringc &Filename = ""
        );
        
        Texture* createScreenShot(const dim::point2di &Position = 0, dim::size2di Size = 0);
        
        /* === Matrix controll === */
        
        void updateModelviewMatrix();
        
        /* === Special renderer functions === */
        
        inline ID3D11Device* getDirect3DDevice() const
        {
            return Device_;
        }
        
    private:
        
        friend class Direct3D11ShaderTable;
        friend class Direct3D11Shader;
        friend class Direct3D11Texture;
        friend class Direct3D11ComputeShaderIO;
        friend class Direct3D11RenderContext;
        friend class VertexFormat;
        
        /* === Structures === */
        
        struct SMeshBufferData // !!!
        {
            SMeshBufferData();
            ~SMeshBufferData();
            
            /* Members */
            u32* BufferID;
            D3D11_BUFFER_DESC VertexDesc, IndexDesc;
            
            u32 VerticesCount;
            ID3D11Buffer* VertexBuffer;
            
            u32 IndicesCount;
            ID3D11Buffer* IndexBuffer;
        };
        
        struct SGeneralBuffer
        {
            SGeneralBuffer() : ElementCount(0), BufferSize(0), HWBuffer(0), IndexFormat(DXGI_FORMAT_R32_UINT)
            {
                ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
            }
            ~SGeneralBuffer()
            {
            }
            
            u32 ElementCount, BufferSize;
            ID3D11Buffer* HWBuffer;
            D3D11_BUFFER_DESC BufferDesc;
            
            DXGI_FORMAT IndexFormat;
        };
        
        struct SConstantBufferLights
        {
            struct SLight
            {
                SLight() : Enabled(0)
                {
                }
                ~SLight()
                {
                }
                
	            s32 Model;                                  // Light model (Directionl, Point, Spot)
	            s32 Enabled;                                // Enabled/ disabled
	            f32 pad1[2];
	            dim::vector4df Position;                    // Position for Point- and Spot light and Direction for Directional light
	            dim::vector4df Diffuse, Ambient, Specular;  // Light colors
	            dim::vector4df SpotDir;                     // Spot light direction
	            f32 Attn0, Attn1, Attn2;                    // Attunation values
	            f32 pad2;
	            f32 Theta, Phi, Falloff, Range;             // Spot light attributes
            };
            
            SLight Lights[8];
        };
        
        struct SConstantBufferObject
        {
            struct SMaterial
            {
                dim::vector4df Diffuse, Ambient, Specular, Emission;    // Material colors
                s32 Shading;                                            // Shading (flat, gouraud, phong, perpixel)
                s32 LightingEnabled;                                    // Global lighting enabled/ disabled
                s32 FogEnabled;                                         // Global fog enabled/ disabled
                f32 Shininess;									        // Specular shininess
                s32 AlphaMethod;                                        // Alpha test function
                f32 AlphaReference;                                     // Alpha test reference value
                s32 pad[2];
            };
            
            dim::matrix4f WorldMatrix, ViewMatrix, ProjectionMatrix;
            SMaterial Material;
        };
        
        struct SConstantBufferSurface
        {
            struct STexture
            {
                dim::vector3di MapGenType;  // Texture coordinate generation
                s32 TexEnvType;             // Texture environment
                dim::matrix4f Matrix;       // Texture coordiante transformation
            };
            
            s32 TextureLayers;
            s32 pad[3];
            STexture Textures[8];
        };
        
        struct SConstantBufferDriverSettings
        {
            struct SClipPlane
            {
                SClipPlane() : Enabled(0)
                {
                }
                ~SClipPlane()
                {
                }
                
                s32 Enabled;            // Enabled/ disabled
                s32 pad[3];
                dim::plane3df Plane;    // Clipping plane
            };
            
            struct SFogStates
            {
                enum EConstBufferFogModes
                {
                    FOGMODE_STATIC_PALE = 0,
                    FOGMODE_STATIC_THICK,
                    FOGMODE_VOLUMETRIC,
                };
                
                s32 Mode;               // Fog mode (Plane, Thick etc.)
                f32 Density;            // Density/ thikness
                f32 Near, Far;          // Near/ far planes
                dim::vector4df Color;   // Fog color
            };
            
            SClipPlane Planes[8];   // Clipping planes;
            SFogStates Fog;         // Fog effect states
        };
        
        struct SConstantBuffer2D
        {
            dim::matrix4f Transformation;
            
            dim::vector4df ColorLeftTop;
            dim::vector4df ColorRightTop;
            dim::vector4df ColorRightBottom;
            dim::vector4df ColorLeftBottom;
            
            dim::vector4df TexCoordLeftTop;
            dim::vector4df TexCoordRightTop;
            dim::vector4df TexCoordRightBottom;
            dim::vector4df TexCoordLeftBottom;
            
            bool IsTexturing;
            s32 pad[3];
        };
        
        /* === Functions === */
        
        void init();
        void clear();
        
        static void setupTextureFormats(
            const EPixelFormats Format, const EHWTextureFormats HWFormat, DXGI_FORMAT &D3DFormat
        );
        
        bool createRendererTexture(
            bool MipMaps, const ETextureDimensions Dimension, dim::vector3di Size,
            const EPixelFormats Format, const u8* ImageBuffer, const EHWTextureFormats HWFormat = HWTEXFORMAT_UBYTE8
        );
        
        /* Extended render functions */
        
        void bindTextureList(const std::vector<SMeshSurfaceTexture> &TextureList);
        void unbindTextureList(const std::vector<SMeshSurfaceTexture> &TextureList);
        
        void updateDefaultBasicShader(const std::vector<SMeshSurfaceTexture> &TextureList);
        void updateConstBufferDriverSettings();
        
        void updateShaderResources();
        
        void createQuad2DVertexBuffer();
        
        ID3D11Buffer* createStructuredBuffer(u32 ElementSize, u32 ElementCount, void* InitData = 0);
        ID3D11Buffer* createCPUAccessBuffer(ID3D11Buffer* GPUOutputBuffer);
        ID3D11UnorderedAccessView* createUnorderedAccessView(ID3D11Buffer* StructuredBuffer);
        ID3D11ShaderResourceView* createShaderResourceView(ID3D11Buffer* StructuredBuffer);
        
        void updateHardwareBuffer(
            SGeneralBuffer* Buffer, const dim::UniversalBuffer &BufferData, const EMeshBufferUsage Usage,
            const D3D11_BIND_FLAG BindFlag, const io::stringc &Name
        );
        
        void updateVertexInputLayout(VertexFormat* Format, bool isCreate);
        void addVertexInputLayoutAttribute(std::vector<D3D11_INPUT_ELEMENT_DESC>* InputDesc, const SVertexAttribute &Attrib);
        
        /* Inline functions */
        
        template <class T> static inline void releaseObject(T* &Object)
        {
            if (Object)
            {
                Object->Release();
                Object = 0;
            }
        }
        
        /* === Private members === */
        
        /* Direct3D members */
        
        ID3D11Device* Device_;
        ID3D11DeviceContext* DeviceContext_;
        
        ID3D11RenderTargetView* RenderTargetView_;
        ID3D11RenderTargetView* OrigRenderTargetView_;
        
        ID3D11Texture2D* DepthStencil_;
        ID3D11DepthStencilView* DepthStencilView_;
        ID3D11DepthStencilView* OrigDepthStencilView_;
        
        ID3D11RasterizerState* RasterizerState_;
        ID3D11DepthStencilState* DepthStencilState_;
        ID3D11BlendState* BlendState_;
        
        ID3D11Texture1D* CurTexture1D_;
        ID3D11Texture2D* CurTexture2D_;
        ID3D11Texture3D* CurTexture3D_;
        
        /* Descriptions */
        
        D3D11_INPUT_ELEMENT_DESC* VertexLayout3D_;
        D3D11_INPUT_ELEMENT_DESC* VertexLayout2D_;
        
        D3D11_RASTERIZER_DESC RasterizerDesc_;
        D3D11_DEPTH_STENCIL_DESC DepthStencilDesc_;
        D3D11_BLEND_DESC BlendDesc_;
        
        /* Containers */
        
        std::vector<SMeshBufferData*> MeshBufferList_;
        
        u32 BindTextureCount_;
        ID3D11ShaderResourceView* ShaderResourceViewList_[MAX_COUNT_OF_TEXTURES];
        ID3D11RenderTargetView* RenderTargetViewList_[MAX_COUNT_OF_TEXTURES];
        ID3D11SamplerState* SamplerStateList_[MAX_COUNT_OF_TEXTURES];
        
        ID3D11Buffer* Quad2DVertexBuffer_;
        
        /* Other members */
        
        bool isFullscreen_;
        f32 FinalClearColor_[4];
        video::color ClearColor_;
        
        D3D_FEATURE_LEVEL FeatureLevel_;
        
        MaterialStates* Material2DDrawing_;
        
        /* Default basic shader objects */
        
        ShaderTable* DefaultBasicShader_;
        bool UseDefaultBasicShader_;
        
        ShaderTable* DefaultBasicShader2D_;
        
        SConstantBufferLights ConstBufferLights_;
        SConstantBufferObject ConstBufferObject_;
        SConstantBufferSurface ConstBufferSurface_;
        SConstantBufferDriverSettings ConstBufferDriverSettings_;
        
        SConstantBuffer2D ConstBuffer2D_;
        
};


} // /namespace video

} // /namespace sp


#endif

#endif



// ================================================================================
