#pragma once

template<typename T>
T Max(T l, T r)
{
	return l >= r ? l : r;
}

template<>
inline Vector3D Max(Vector3D l, Vector3D r)
{
	return Vector3D(
		Max((float) l.x, (float) r.x),
		Max((float) l.y, (float) r.y),
		Max((float) l.z, (float) r.z)
	);
}

template<typename T>
T Min(T l, T r)
{
	return l > r ? r : l;
}

template<>
inline Vector3D Min(Vector3D l, Vector3D r)
{
	return Vector3D(
		Min((float) l.x, (float) r.x),
		Min((float) l.y, (float) r.y),
		Min((float) l.z, (float) r.z)
	);
}
