#pragma once

template<typename T>
T Max(T l, T r)
{
	return l >= r ? l : r;
}

template<typename T>
T Min(T l, T r)
{
	return l > r ? r : l;
}
