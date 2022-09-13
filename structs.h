#pragma once

#include <Windows.h>
#include <locale>

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int Count;
	int Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FName
{
	int32_t ComparisonIndex;
	int32_t Number;
};

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} D3DMATRIX;

struct FVector
{
	float x, y, z;

	FVector() {
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	FVector(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline float Dot(FVector v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	FVector operator+(FVector v)
	{
		return FVector(x + v.x, y + v.y, z + v.z);
	}

	FVector operator-(FVector v)
	{
		return FVector(x - v.x, y - v.y, z - v.z);
	}
};

struct FVector2D
{
	float x, y;

	FVector2D() {}

	FVector2D(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

struct FRotator
{
	float pitch, yaw, roll;

	FRotator() {
		this->pitch = 0.0f;
		this->yaw = 0.0f;
		this->roll = 0.0f;
	}

	FRotator(float pitch, float yaw, float roll) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
	}
};

struct FMinimalViewInfo
{
	FVector Location;
	FRotator Rotation;
	float FOV;
	float DesiredFOV;
	char _pad01[2000];
};

struct FPlane
{
	float x;
	float y;
	float z;
	float w;
};

struct FMatrix
{
	struct FPlane XPlane;
	struct FPlane YPlane;
	struct FPlane ZPlane;
	struct FPlane WPlane;
};

typedef struct {
	FVector ViewOrigin;
	char _padding_0[4];
	FMatrix ViewRotationMatrix;
	FMatrix ProjectionMatrix;
} FSceneViewProjectionData;

typedef struct {
	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
} FBoxSphereBounds;
