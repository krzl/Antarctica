#pragma once

namespace Renderer
{
	class NativeComputeShader;
	class NativeShader;
	class NativeSubmesh;
	class NativeTexture;
	class NativeBuffer;
	
	extern void Deleter(NativeComputeShader*);
	extern void Deleter(NativeShader*);
	extern void Deleter(NativeSubmesh*);
	extern void Deleter(NativeTexture*);
	extern void Deleter(NativeBuffer*);
}