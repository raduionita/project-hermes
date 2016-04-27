#ifndef __cbuffer_hpp__
#define __cbuffer_hpp__

namespace core
{
  template <typename T>
  class CBuffer
  {
    protected:
    T*     mBuffer;
    size_t mSize;
    
    public:
    CBuffer(size_t size) : mBuffer(new T[size]), mSize(size)
    {
      
    }
    
    virtual ~CBuffer()
    {
      delete [] mBuffer;
    }
    
    operator T* ()
    {
      return mBuffer;
    }
    
    operator const T* () const 
    {
      return mBuffer;
    }
    
    public:
    CBuffer& append(T* data, size_t size)
    {
      
    }
    
    size_t getSize() const
    {
      return mSize;
    }
  };
}

#endif // __cbuffer_hpp__
