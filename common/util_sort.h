#ifndef UTIL_SORT_H
#define UTIL_SORT_H

namespace util
{

/*
insertionSort(array A)
begin
    for i := 1 to length[A] - 1 do
    begin
        value := A[i];
        j := i - 1;
        while j >= 0 and A[j] > value do
        begin
            A[j + 1] := A[j];
            j := j - 1;
        end;
        A[j + 1] := value;
    end;
end;
*/

template <typename T> inline void InsertionSort(T *list, int count)
{
	InsertionSort(list, count,
		[] (const T &a, const T &b)
	{
		return a > b;
	});
}

template <typename T, typename GTFunc> inline void InsertionSort(T *list, int count, GTFunc &gtFunc)
{
	for (int i = 1; i < count; i++)
	{
		T value = list[i];
		int j = i - 1;
		while (j >= 0 && gtFunc(list[j], value))
		{
			list[j + 1] = list[j];
			j = j - 1;
		}
		list[j + 1] = value;
	}
}

} // namespace util

#endif
