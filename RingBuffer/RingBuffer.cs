using System;
using System.Collections;

public class RingBuffer<T> : IEnumerator, IEnumerable
{
    private T[] data = null;
    private int newestWrittenIndex = -1;
    private int oldestValidIndex = -1;
    private int numElements = 0;
    private int position = -1; // position for IEnumerator

    public int Length { get => numElements; }

    public object Current => data[position];

    public T this[int i]
    {
        get
        {
            if (i < 0 || i >= Length)
            {
                throw new IndexOutOfRangeException();
            }
            return data[(oldestValidIndex + i) % data.Length];
        }
    }

    public RingBuffer(int size)
    {
        data = new T[size];
    }

    public void Clear()
    {
        data = new T[data.Length];
        newestWrittenIndex = oldestValidIndex = -1;
        numElements = 0;
    }

    public void Save(T element)
    {
        newestWrittenIndex = (newestWrittenIndex + 1) % data.Length; // wrap-around
        data[newestWrittenIndex] = element;
        if (oldestValidIndex == -1)
        {
            oldestValidIndex = 0;
        } else if (oldestValidIndex == newestWrittenIndex)
        {
            oldestValidIndex = (oldestValidIndex + 1) % data.Length;
        }
        numElements = Math.Min(numElements + 1, data.Length);
    }

    public bool MoveNext()
    {
        if (position == newestWrittenIndex)
        {
            position = -1;
            return false;
        }
        if (position == -1)
        {
            position = oldestValidIndex;
        }
        else
        {
            position = (position + 1) % data.Length;
        }
        return true;
    }

    public void Reset()
    {
        position = -1;
    }

    public IEnumerator GetEnumerator()
    {
        return this;
    }
}
