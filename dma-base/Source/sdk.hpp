#pragma once
#include <d3d9.h>
#include <vector>
#include <offsets.hpp>
#include <settings.hpp>
#define M_PI 3.14159265358979323846264338327950288419716939937510


enum perfect_skeleton {
	BONE_BOTTOM = 0,
	BONE_HEAD = 110,
	BONE_NECK = 66, // 67
	BONE_NECK2 = 8,
	BONE_CHEST = 7,
	BONE_PELVIS = 2,
	BONE_RSHOULDER = 38,
	BONE_RELBOW = 39,
	BONE_RHAND = 40,
	BONE_RTHIGH = 78,
	BONE_RKNEE = 79,
	BONE_RFOOT = 82,
	BONE_RFOOTOUT = 83,
	BONE_LSHOULDER = 9,
	BONE_LELBOW = 10,
	BONE_LHAND = 11,
	BONE_LTHIGH = 71,
	BONE_LKNEE = 72,
	BONE_LFOOT = 75,
	BONE_LFOOTOUT = 76
};

enum bone : int
{
	HumanBase = 0,
	HumanPelvis = 2,
	HumanLThigh1 = 71,
	HumanLThigh2 = 77,
	HumanLThigh3 = 72,
	HumanLCalf = 74,
	HumanLFoot2 = 75, // 73
	HumanLFoot = 76, // 86
	HumanLToe = 76,
	HumanRThigh1 = 78,
	HumanRThigh2 = 84,
	HumanRThigh3 = 79,
	HumanRCalf = 81,
	HumanRFoot2 = 82,
	HumanRFoot = 82, // 87
	HumanRToe = 83,
	HumanSpine1 = 7,
	HumanSpine2 = 5,
	HumanSpine3 = 2,
	HumanLCollarbone = 9, // 9
	HumanLUpperarm = 35,
	HumanLForearm1 = 36,
	HumanLForearm23 = 10,
	HumanLForearm2 = 34,
	HumanLForearm3 = 33,
	HumanLPalm = 32,
	HumanLHand = 11,
	HumanRCollarbone = 38, // 98
	HumanRUpperarm = 64,
	HumanRForearm1 = 65,
	HumanRForearm23 = 39,
	HumanRForearm2 = 63,
	HumanRForearm3 = 62,
	HumanRHand = 40,
	HumanRPalm = 58,
	HumanNeck = 66,
	HumanHead = 110,
	HumanLowerHead = 106,
	HumanChest = 65
};

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(double _x, double _y) : x(_x), y(_y) {}
	~Vector2() {}
	double x, y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	~Vector3() {}
	double x, y, z;
	inline double dot(Vector3 v) { return x * v.x + y * v.y + z * v.z; }
	inline double distance(Vector3 v) { return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0))); }
	Vector3 operator-(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
};

struct FQuat { double x, y, z, w; };
struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX to_matrix_with_scale()
	{
		D3DMATRIX m{};
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;
		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;
		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;
		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;
		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;
		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;
		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;
		return m;
	}
};

D3DMATRIX matrix_multiplication(D3DMATRIX pm1, D3DMATRIX pm2)
{
	D3DMATRIX pout{};
	pout._11 = pm1._11 * pm2._11 + pm1._12 * pm2._21 + pm1._13 * pm2._31 + pm1._14 * pm2._41;
	pout._12 = pm1._11 * pm2._12 + pm1._12 * pm2._22 + pm1._13 * pm2._32 + pm1._14 * pm2._42;
	pout._13 = pm1._11 * pm2._13 + pm1._12 * pm2._23 + pm1._13 * pm2._33 + pm1._14 * pm2._43;
	pout._14 = pm1._11 * pm2._14 + pm1._12 * pm2._24 + pm1._13 * pm2._34 + pm1._14 * pm2._44;
	pout._21 = pm1._21 * pm2._11 + pm1._22 * pm2._21 + pm1._23 * pm2._31 + pm1._24 * pm2._41;
	pout._22 = pm1._21 * pm2._12 + pm1._22 * pm2._22 + pm1._23 * pm2._32 + pm1._24 * pm2._42;
	pout._23 = pm1._21 * pm2._13 + pm1._22 * pm2._23 + pm1._23 * pm2._33 + pm1._24 * pm2._43;
	pout._24 = pm1._21 * pm2._14 + pm1._22 * pm2._24 + pm1._23 * pm2._34 + pm1._24 * pm2._44;
	pout._31 = pm1._31 * pm2._11 + pm1._32 * pm2._21 + pm1._33 * pm2._31 + pm1._34 * pm2._41;
	pout._32 = pm1._31 * pm2._12 + pm1._32 * pm2._22 + pm1._33 * pm2._32 + pm1._34 * pm2._42;
	pout._33 = pm1._31 * pm2._13 + pm1._32 * pm2._23 + pm1._33 * pm2._33 + pm1._34 * pm2._43;
	pout._34 = pm1._31 * pm2._14 + pm1._32 * pm2._24 + pm1._33 * pm2._34 + pm1._34 * pm2._44;
	pout._41 = pm1._41 * pm2._11 + pm1._42 * pm2._21 + pm1._43 * pm2._31 + pm1._44 * pm2._41;
	pout._42 = pm1._41 * pm2._12 + pm1._42 * pm2._22 + pm1._43 * pm2._32 + pm1._44 * pm2._42;
	pout._43 = pm1._41 * pm2._13 + pm1._42 * pm2._23 + pm1._43 * pm2._33 + pm1._44 * pm2._43;
	pout._44 = pm1._41 * pm2._14 + pm1._42 * pm2._24 + pm1._43 * pm2._34 + pm1._44 * pm2._44;
	return pout;
}

D3DMATRIX to_matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radpitch = (rot.x * M_PI / 180);
	float radyaw = (rot.y * M_PI / 180);
	float radroll = (rot.z * M_PI / 180);
	float sp = sinf(radpitch);
	float cp = cosf(radpitch);
	float sy = sinf(radyaw);
	float cy = cosf(radyaw);
	float sr = sinf(radroll);
	float cr = cosf(radroll);
	D3DMATRIX matrix{};
	matrix.m[0][0] = cp * cy;
	matrix.m[0][1] = cp * sy;
	matrix.m[0][2] = sp;
	matrix.m[0][3] = 0.f;
	matrix.m[1][0] = sr * sp * cy - cr * sy;
	matrix.m[1][1] = sr * sp * sy + cr * cy;
	matrix.m[1][2] = -sr * cp;
	matrix.m[1][3] = 0.f;
	matrix.m[2][0] = -(cr * sp * cy + sr * sy);
	matrix.m[2][1] = cy * sr - cr * sp * sy;
	matrix.m[2][2] = cr * cp;
	matrix.m[2][3] = 0.f;
	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;
	return matrix;
}

struct Camera
{
	Vector3 location;
	Vector3 rotation;
	float fov;
};

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

namespace cache
{
	inline uintptr_t uworld;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;
	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline Vector3 relative_location;
	inline int my_team_id;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline int player_count;
	inline float closest_distance;
	inline uintptr_t closest_mesh;
	inline Camera local_camera;
	inline uintptr_t base;
	inline Vector2 head2d;
	inline Vector2 neck2d;
	inline Vector2 hitbox_screen_predict;
}

Camera get_view_point()
{
	Camera view_point{};
	uintptr_t location_pointer;
	uintptr_t rotation_pointer;
	location_pointer = mem.Read<uintptr_t>(cache::uworld + 0x110);
	rotation_pointer = mem.Read<uintptr_t>(cache::uworld + 0x120);
	FNRot fnrot{};
	fnrot.a = mem.Read<double>(rotation_pointer);
	fnrot.b = mem.Read<double>(rotation_pointer + 0x20);
	fnrot.c = mem.Read<double>(rotation_pointer + 0x1D0);
	view_point.location = mem.Read<Vector3>(location_pointer);
	view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
	view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
	view_point.fov = mem.Read<float>(cache::player_controller + 0x394) * 90.f;
	return view_point;
}

Vector2 project_world_to_screen(Vector3 world_location)
{
	cache::local_camera = get_view_point();
	D3DMATRIX temp_matrix = to_matrix(cache::local_camera.rotation);
	Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - cache::local_camera.location;
	Vector3 vtransformed = Vector3(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
	if (vtransformed.z < 1) vtransformed.z = 1;
	return Vector2(settings::screen_center_x + vtransformed.x * ((settings::screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z, settings::screen_center_y - vtransformed.y * ((settings::screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z);
}

Vector3 get_entity_bone(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = mem.Read<uintptr_t>(mesh + offsets::BONE_ARRAY);
	if (bone_array == 0) bone_array = mem.Read<uintptr_t>(mesh + offsets::BONE_ARRAY_CACHE);
	FTransform bone = mem.Read<FTransform>(bone_array + (bone_id * 0x60));
	FTransform component_to_world = mem.Read<FTransform>(mesh + offsets::COMPONENT_TO_WORLD);
	D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

static float powf_(float _X, float _Y) {
	return (_mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y))));
}
static float sqrtf_(float _X) {
	return (_mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(_X))));
}
static double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrtf(powf((x2 - x1), 2) + powf_((y2 - y1), 2));
}

bool is_visible(uintptr_t mesh)
{
	float last_sumbit_time = mem.Read<float>(mesh + offsets::LAST_SUMBIT_TIME);
	float last_render_time_on_screen = mem.Read<float>(mesh + offsets::LAST_SUMBIT_TIME_ON_SCREEN);
	return last_render_time_on_screen + 0.06f >= last_sumbit_time;
}
bool kmBox::init()
{
	std::string port = kmBox::FindPort("USB-SERIAL CH340");
	if (port.empty())
	{
		printf("[KMBOX] failed to find port!\n");
		return false;
	}

	if (!kmBox::OpenPort(hSerial, port.c_str(), CBR_115200))
	{
		printf("[KMBOX] failed to open port!\n");
		return false;
	}

	printf("[KMBOX] created successfully!\n");

	return true;
}

void kmBox::sendMove(int x, int y)
{
	std::stringstream commandStream;
	commandStream << "km.move(" << x << "," << y << ")\r\n";
	SendCommand(hSerial, commandStream.str());
}

Vector3 Prediction(Vector3 TargetPosition, Vector3 ComponentVelocity, float player_distance, float ProjectileSpeed = 239)
{
		float gravity = 3.5;
		float TimeToTarget = player_distance / ProjectileSpeed;
		float bulletDrop = abs(gravity) * (TimeToTarget * TimeToTarget) * 0.5;
		return Vector3
		{
		TargetPosition.x + TimeToTarget * ComponentVelocity.x,
		TargetPosition.y + TimeToTarget * ComponentVelocity.y,
		TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
		};

}

auto move(float x, float y) -> void {
	float screen_center_x = settings::screen_center_x;
	float screen_center_y = settings::screen_center_y;
	Vector3 target(0, 0, 0);

	if (x != 0) {
		if (x > screen_center_x) {
			target.x = -(screen_center_x - x);
			target.x /= (settings::aimbot::smoothness);
			if (target.x + screen_center_x > screen_center_x * 2)
				target.x = 0;
		}

		if (x < screen_center_x) {
			target.x = x - screen_center_x;
			target.x /= (settings::aimbot::smoothness);
			if (target.x + screen_center_x < 0)
				target.x = 0;
		}
	}
	if (y != 0) {
		if (y > settings::screen_center_y) {
			target.y = -(settings::screen_center_y - y);
			target.y /= (settings::aimbot::smoothness);
			if (target.y + settings::screen_center_y > settings::screen_center_y * 2)
				target.y = 0;
		}

		if (y < settings::screen_center_y) {
			target.y = y - settings::screen_center_y;
			target.y /= (settings::aimbot::smoothness);
			if (target.y + settings::screen_center_y < 0)
				target.y = 0;
		}
	}

	const float snapThreshold = 1.0f;
	if (std::abs(target.x) < snapThreshold) {
		target.x = 0;
	}
	if (std::abs(target.y) < snapThreshold) {
		target.y = 0;
	}
	if (settings::kmbox::kmboxnet && settings::aimbot::enable)
	{
		kmNet_mouse_move(target.x, target.y);
	}
	if (settings::kmbox::kmboxb && settings::aimbot::enable)
	{
		kmBox::sendMove(target.x, target.y);
	}

}