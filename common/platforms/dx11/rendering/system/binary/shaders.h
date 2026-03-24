////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shaders.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Binary file shader structures
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Staging buffer notes:
////
////        The staging buffers are ordered as follows:
////
////            constant buffers (offsets and strides used)
////            srvs
////            uavs (offsets used for initial count)
////            sampler states
////            stream-out buffers (offsets and param2 used)
////            stream buffers (offsets and strides used)
////
////        The ordering is chosen to group the StageChain resources for consistent indexing when referenced by
////        multiple ShaderDesc shaders.
////
////        Stream and stream-out buffers are last in the staging buffer because they can have varying numbers
////        of resources.
////
////        All resources have an entry in the buffers, offsets and strides staging arrays, but not all are used.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    start of include guard
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BINARY_SHADERS_INCLUDED__
#define __BINARY_SHADERS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include <d3dcommon.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin binary namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace binary
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader binding types and structures
////
////    Provides the main mapping between resources, staging buffers and hardware for the shader stack
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  Shader stage index enumeration
enum EShaderStage
{
    EShaderStageCompute = 0,
    EShaderStagePixel,
    EShaderStageVertex,
    EShaderStageHull,
    EShaderStageDomain,
    EShaderStageGeometry,
    EShaderStageCount,
    EShaderStageForce32 = 0x7fffffff
};
 
//  Shader stage bit flags enumeration
enum EShaderStageBits
{
    EShaderStageComputeBit     = ( 1 << EShaderStageCompute ),
    EShaderStagePixelBit       = ( 1 << EShaderStagePixel ),
    EShaderStageVertexBit      = ( 1 << EShaderStageVertex ),
    EShaderStageHullBit        = ( 1 << EShaderStageHull ),
    EShaderStageDomainBit      = ( 1 << EShaderStageDomain ),
    EShaderStageGeometryBit    = ( 1 << EShaderStageGeometry ),
    EShaderStageBitForce32     = 0x7fffffff
};

//  ResourceValidation::packed layout:
//      bits 0:3    -    4 bits -   D3D_SHADER_INPUT_TYPE
//      bits 4:8    -    5 bits -   D3D_SHADER_INPUT_FLAGS
//      bits 9:10   -    2 bits -   D3D_CBUFFER_TYPE
//      bit  11     -    1 bit  -   D3D_SHADER_CBUFFER_FLAGS
//      bits 12:15  -    4 bits -   D3D_SRV_DIMENSION
//      bits 16:19  -    4 bits -   D3D_RESOURCE_RETURN_TYPE
//      bits 20:31  -   12 bits -   sample count (if not a texture or not multi-sampled, this is 0)
struct ResourceValidation
{
    uint32_t                        packed;
    inline D3D_SHADER_INPUT_TYPE    getInputType() const { return( static_cast< D3D_SHADER_INPUT_TYPE >( packed & 0x0000000f ) ); };
    inline UINT                     getInputFlags() const { return( static_cast< UINT >( ( packed & 0x000001f0 ) >> 4 ) ); };
    inline D3D_CBUFFER_TYPE         getCBufferType() const { return( static_cast< D3D_CBUFFER_TYPE >( ( packed & 0x00000600 ) >> 9 ) ); };
    inline UINT                     getCBufferFlags() const { return( static_cast< UINT >( ( packed & 0x00000800 ) >> 11 ) ); };
    inline D3D_SRV_DIMENSION        getSRVDimension() const { return( static_cast< D3D_SRV_DIMENSION >( ( packed & 0x0000f000 ) >> 12 ) ); };
    inline D3D_RESOURCE_RETURN_TYPE getReturnType() const { return( static_cast< D3D_RESOURCE_RETURN_TYPE >( ( packed & 0x000f0000 ) >> 16 ) ); };
    inline UINT                     getSampleCount() const { return( static_cast< UINT >( ( packed & 0xfff00000 ) >> 20 ) ); };
    inline void                     setInputType( const D3D_SHADER_INPUT_TYPE inputType ) { packed = ( ( packed & ~0x0000000f ) | ( static_cast< uint >( inputType ) & 0x0000000f ) ); };
    inline void                     setInputFlags( const UINT inputFlags ) { packed = ( ( packed & ~0x000001f0 ) | ( ( static_cast< uint >( inputFlags ) << 4 ) & 0x000001f0 ) ); };
    inline void                     setCBufferType( const D3D_CBUFFER_TYPE cbufferType ) { packed = ( ( packed & ~0x00000600 ) | ( ( static_cast< uint >( cbufferType ) << 9 ) & 0x00000600 ) ); };
    inline void                     setCBufferFlags( const UINT cbufferFlags ) { packed = ( ( packed & ~0x00000800 ) | ( ( static_cast< uint >( cbufferFlags ) << 11 ) & 0x00000800 ) ); };
    inline void                     setSRVDimension( const D3D_SRV_DIMENSION srvDimension ) { packed = ( ( packed & ~0x0000f000 ) | ( ( static_cast< uint >( srvDimension ) << 12 ) & 0x0000f000 ) ); };
    inline void                     setReturnType( const D3D_RESOURCE_RETURN_TYPE returnType ) { packed = ( ( packed & ~0x000f0000 ) | ( ( static_cast< uint >( returnType ) << 16 ) & 0x000f0000 ) ); };
    inline void                     setSampleCount( const UINT sampleCount ) { packed = ( ( packed & ~0xfff00000 ) | ( ( static_cast< uint >( sampleCount ) << 20 ) & 0xfff00000 ) ); };
};

union ResourceDesc
{
    uint32_t                streamFormatIndex;                      //  StreamFormat index within the main StreamFormat array (EResourceTypeStream and EResourceTypeStreamOut)
    ResourceValidation      resourceValidation;                     //  reflected validation data (EResourceTypeCBuffer, EResourceTypeSRV and EResourceTypeUAV)
};

//  resourceItems array member structure
struct ResourceItem
{
    uint32_t                nameHandle;                             //  index of the bound name from the shader reflection or shaders.json file
    uint32_t                resourceType;                           //  EResourceType
    ResourceDesc            desc;                                   //  type specific data
    uint16_t                stride;                                 //  stride or size
    uint16_t                slots;                                  //  slot count for array resources
};

//  streamFormats array member structure
struct StreamFormat
{
    uint32_t                nameHandle;                             //  index of the stream name from the shaders.json file "streams" section
    uint32_t                stride;                                 //  stride of the stream in bytes
    ListDesc                elements;                               //  list of StreamElement structures within the main streamElements array
};

//  streamElements array member structure
struct StreamElement
{
    uint32_t                nameHandle;                             //  index of the element name from the shaders.json file stream description
    uint32_t                format;                                 //  rendering::EElementFormat
    uint32_t                size;                                   //  size of the element in bytes
    uint32_t                offset;                                 //  offset of the element from the start of the stream in bytes
};

//  assemblyLayouts array member structure
struct AssemblyLayout
{
    uint32_t                shaderBinaryIndex;                      //  index of the BlobDesc of the shader binary within the main shaderBlobs array
    uint32_t                rasterisedStreamIndex;                  //  stream to be used for rasterisation (0xffffffff if no rasterisation, ignored if not stream-out)
    ListDesc                streams;                                //  list of AssemblyStream structures within the main assemblyStreams array
};

//  assemblyStreams array member structure
struct AssemblyStream
{
    uint32_t                streamFormatIndex;                      //  index of the StreamFormat referenced by the AssemblyElement array
    uint32_t                instanceStepRate;                       //  instancing step rate (ignored if stream-out)
    ListDesc                elements;                               //  list of AssemblyElement structures within the main assemblyElements array
};

//  assemblyElements array member structure
struct AssemblyElement
{
    uint32_t                streamElementIndex;                     //  index of the StreamElement within the StreamLayout specified by the AssemblyStream
    uint32_t                semanticHandle;                         //  index of the semantic name
    uint32_t                semanticIndex;                          //  index of the semantic
    uint32_t                componentIndex;                         //  stream-out starting component (component count is implicit in the target element size)
};

//  stagingScatters array member structure
struct StagingScatter
{   //  one to many mapping (cbuffers, samplers, srvs, uavs)
    uint32_t                instances;                              //  number of instances of the resource to replicate in the staging array
    uint32_t                locations;                              //  starting index of the staging index array within the main stagingIndices array
};

//  stagingGathers array member structure
struct StagingGather
{   //  many to many mapping (cbuffers, samplers, srvs, uavs)
    uint32_t                slotCount;                              //  count of hardware slots to set
    uint32_t                firstSlot;                              //  index of the first hardware slot
};

struct Scattering
{   //  note: the target count is the number of resources to scatter
    uint32_t                source;                                 //  starting index of the Shader resources to map into the staging array
    ListDesc                target;                                 //  list of StagingScatter structures within the main stagingScatters array
};

struct Gathering
{   //  note: the target count is the number of contiguous slot blocks for a shader stage
    uint32_t                source;                                 //  starting index of the resources in the staging array
    ListDesc                target;                                 //  list of StagingGather structures within the main stagingGathers array
};

struct Remapping
{   //  many to many mapping (streams, streamOuts and rtvs)
    uint32_t                source;                                 //  starting index of the Shader resources to map into the staging array
    Indexing                target;                                 //  list of staging array offsets
};

//  stageChains array member structure
struct StageChain
{
    uint32_t                nameHandle;                             //  index of the shader chain name
    uint32_t                stageMask;                              //  flags for active shader stages (a combination of EShaderStageBits)
    uint32_t                shaderIndices[ EShaderStageCount ];     //  indices of the shader stage interfaces (within the ShaderLibrary object)
    Scattering              bufferScattering;                       //  buffer resource data scattering description
    Gathering               bufferGathering[ EShaderStageCount ];   //  buffer resource data gathering description
    Scattering              srvScattering;                          //  shader resource view resource data scattering description
    Gathering               srvGathering[ EShaderStageCount ];      //  shader resource view resource data gathering description
    Scattering              uavScattering;                          //  unordered access view scattering description
    Gathering               uavGathering[ EShaderStageCount ];      //  unordered access view gathering description
    Scattering              samplerScattering;                      //  sampler state resource data scattering description
    Gathering               samplerGathering[ EShaderStageCount ];  //  sampler state resource data gathering description
};

//  shaderDescs array member structure
struct ShaderDesc
{
    uint32_t                nameHandle;                             //  index of the shader name
    uint32_t                inputIndex;                             //  input assembler index (within the ShaderLibrary object)
    uint32_t                outputIndex;                            //  stream-out geometry shader index (within the ShaderLibrary object)
    uint32_t                chainIndex;                             //  index of the StageChain
    uint32_t                stagingCount;                           //  staging buffer array element count
    ListDesc                resources;                              //  list of resources within the main resourceIndices array
    uint32_t                rasterStateIndex;                       //  raster state resource index
    uint32_t                depthStateIndex;                        //  depth state resource index
    uint32_t                blendStateIndex;                        //  blend state resource index
    uint32_t                dsvIndex;                               //  depth stencil view resource index
    uint32_t                rtvIndex;                               //  render target view resource first index
    ListDesc                rtvRemap;                               //  render target view resource remapping
    Remapping               streams;                                //  stream in remapping
    Remapping               outputs;                                //  stream out remapping
};

struct ShaderLibraryIndex
{
    SectionDesc             stringTable;                            //  description of the FrozenStringTable section
    SectionDesc             shaderBinaries;                         //  description of the shaderBinaries section
    ArrayDesc               shaderBlobs;                            //  description of the shaderBlobs array
    ArrayDesc               resourceItems;                          //  description of the resourceItems array
    ArrayDesc               resourceIndices;                        //  description of the resourceIndices array
    ArrayDesc               streamFormats;                          //  description of the streamFormats array
    ArrayDesc               streamElements;                         //  description of the streamElements array
    ArrayDesc               assemblyLayouts;                        //  description of the assemblyLayouts array
    ArrayDesc               assemblyStreams;                        //  description of the assemblyStreams array
    ArrayDesc               assemblyElements;                       //  description of the assemblyElements array
    ArrayDesc               stagingScatters;                        //  description of the stagingScatters array
    ArrayDesc               stagingGathers;                         //  description of the stagingGathers array
    ArrayDesc               stagingIndices;                         //  description of the stagingIndices array
    ArrayDesc               stageChains;                           //  description of the stageChains array
    ArrayDesc               shaderDescs;                           //  description of the shaderDescs array
};

struct ShaderLibrarySetup
{
    uint32_t                interfaceCount;                         //  count of interface pointers that need to be allocated for shaders, input layouts and stream-out layouts
    uint32_t                maxStagingCount;                        //  maximum staging buffer array element count in any ShaderDesc in the library
    uint32_t                maxStreamElements;                      //  maximum number of elements required for input layout interface creation
    uint32_t                maxStreamOutElements;                   //  maximum number of elements required for stream-out layout interface creation
    ListDesc                shaderStages[ EShaderStageCount ];      //  references the shaderBlobs array
    ListDesc                inputAssemblies;                        //  references the assemblyLayouts array for use with CreateInputLayout
    ListDesc                outputAssemblies;                       //  references the assemblyLayouts array for use with CreateGeometryShaderWithStreamOutput
};

struct ShaderLibrary
{
    FileHeader              header;                                 //  magic file identifier 'isSL'
    ShaderLibraryIndex      index;                                  //  file contents locations
    ShaderLibrarySetup      setup;                                  //  configuration information
};

union ShaderBinaryPtr
{
    ShaderLibrary*         library;                                 //  shader library
    uint32_t*              data;                                    //  SectionDesc and ArrayDesc are relative to this
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end binary namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace binary

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of include guard
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __BINARY_SHADERS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
