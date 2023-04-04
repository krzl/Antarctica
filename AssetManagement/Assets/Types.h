#pragma once

namespace Renderer
{
	class IComputeShader;
	class IShader;
	class ISubmesh;
	class ITexture;
	class IBuffer;
	
	extern void Deleter(IComputeShader*);
	extern void Deleter(IShader*);
	extern void Deleter(ISubmesh*);
	extern void Deleter(ITexture*);
	extern void Deleter(IBuffer*);
}