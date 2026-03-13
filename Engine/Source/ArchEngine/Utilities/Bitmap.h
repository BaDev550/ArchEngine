#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>
#include "DataBuffer.h"

namespace ae {
    enum class BitmapFormat {
        UnsignedByte,
        Float,
    };

    enum class BitmapType {
        T2D,
        Cube
    };

    static int GetBytesPerComponent(BitmapFormat Fmt) {
        switch (Fmt) {
        case BitmapFormat::UnsignedByte: return 1;
        case BitmapFormat::Float: return 4;
        };
    }

    class Bitmap {
    public:
        Bitmap() {};

        void Init(int w, int h, int comp, BitmapFormat fmt) {
            _w = w;
            _h = h;
            _comp = comp;
            fmt_ = fmt;

            uint64_t size = (uint64_t)w * h * comp * GetBytesPerComponent(fmt);
            _data.Allocate(size);
            initGetSetFuncs();
        }

        Bitmap(int w, int h, int comp, BitmapFormat fmt)
            :_w(w), _h(h), _comp(comp), fmt_(fmt)
        {
            uint64_t size = (uint64_t)w * h * comp * GetBytesPerComponent(fmt);
            _data.Allocate(size);
            initGetSetFuncs();
        }

        Bitmap(int w, int h, int d, int comp, BitmapFormat fmt)
            :_w(w), _h(h), _d(d), _comp(comp), fmt_(fmt)
        {
            uint64_t size = (uint64_t)w * h * d * comp * GetBytesPerComponent(fmt);
            _data.Allocate(size);
            initGetSetFuncs();
        }

        Bitmap(int w, int h, int comp, BitmapFormat fmt, void* ptr)
            :_w(w), _h(h), _comp(comp), fmt_(fmt)
        {
            uint64_t size = (uint64_t)w * h * comp * GetBytesPerComponent(fmt);
            _data.Allocate(size);
            initGetSetFuncs();
            memcpy(_data.Data, ptr, _data.Size);
        }
        Bitmap(DataBuffer data) {
            _data = data;
            _w = 1;
            _h = 1;
            _comp = 4;
        }
        void Release() {
            if (_data)
                _data.Release();
        }

        int _w = 0;
        int _h = 0;
        int _d = 1;
        int _comp = 3;
        DataBuffer _data;
        BitmapFormat fmt_ = BitmapFormat::UnsignedByte;
        BitmapType type_ = BitmapType::T2D;

        void setPixel(int x, int y, glm::vec4& c) {
            (*this.*setPixelFunc)(x, y, c);
        }

        glm::vec4 getPixel(int x, int y) const {
            return ((*this.*getPixelFunc)(x, y));
        }
    private:
        using setPixel_t = void(Bitmap::*)(int, int, glm::vec4&);
        using getPixel_t = glm::vec4(Bitmap::*)(int, int) const;
        setPixel_t setPixelFunc = &Bitmap::setPixelUnsignedByte;
        getPixel_t getPixelFunc = &Bitmap::getPixelUnsignedByte;

        void initGetSetFuncs() {
            switch (fmt_) {
            case BitmapFormat::UnsignedByte:
                setPixelFunc = &Bitmap::setPixelUnsignedByte;
                getPixelFunc = &Bitmap::getPixelUnsignedByte;
                break;
            case BitmapFormat::Float:
                setPixelFunc = &Bitmap::setPixelFloat;
                getPixelFunc = &Bitmap::getPixelFloat;
                break;
            }
        }

        void setPixelFloat(int x, int y, glm::vec4& c) {
            int ofs = _comp * (y * _w + x);
            float* data = (float*)(_data.Data);
            if (_comp > 0) data[ofs + 0] = c.x;
            if (_comp > 1) data[ofs + 1] = c.y;
            if (_comp > 2) data[ofs + 2] = c.z;
            if (_comp > 3) data[ofs + 3] = c.w;
        }

        glm::vec4 getPixelFloat(int x, int y) const {
            int ofs = _comp * (y * _w + x);
            const float* data = (const float*)(_data.Data);
            return glm::vec4(
                _comp > 0 ? data[ofs + 0] : 0.0f,
                _comp > 1 ? data[ofs + 1] : 0.0f,
                _comp > 2 ? data[ofs + 2] : 0.0f,
                _comp > 3 ? data[ofs + 3] : 0.0f);
        }

        void setPixelUnsignedByte(int x, int y, glm::vec4& c) {
            int ofs = _comp * (y * _w + x);
            if (_comp > 0) _data.Data[ofs + 0] = uint8_t(c.x * 255.0f);
            if (_comp > 1) _data.Data[ofs + 1] = uint8_t(c.y * 255.0f);
            if (_comp > 2) _data.Data[ofs + 2] = uint8_t(c.z * 255.0f);
            if (_comp > 3) _data.Data[ofs + 3] = uint8_t(c.w * 255.0f);
        }

        glm::vec4 getPixelUnsignedByte(int x, int y) const {
            int ofs = _comp * (y * _w + x);
            return glm::vec4(
                _comp > 0 ? float(_data.Data[ofs + 0]) / 255.0f : 0.0f,
                _comp > 1 ? float(_data.Data[ofs + 1]) / 255.0f : 0.0f,
                _comp > 2 ? float(_data.Data[ofs + 2]) / 255.0f : 0.0f,
                _comp > 3 ? float(_data.Data[ofs + 3]) / 255.0f : 0.0f);
        }
    };
}