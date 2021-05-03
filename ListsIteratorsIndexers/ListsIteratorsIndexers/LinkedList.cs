using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ListsIteratorsIndexers
{
    class LinkedList<T> : IEnumerable<T>
    {      
        public int Length { get => numElements; }

        // Achtung: Der Indexer sollte auf keinen Fall in einer Schleife
        //          verwendet werden!
        public T this[int i]
        {
            get
            {
                if (i < 0 || i >= numElements)
                {
                    throw new IndexOutOfRangeException();
                }
                Node current = head;
                for (int j = 0; j < i; j++)
                {
                    current = current.Next;
                }
                return current.Value;
            }
            set
            {
                if (i < 0 || i >= numElements)
                {
                    throw new IndexOutOfRangeException();
                }
                Node current = head;
                for (int j = 0; j < i; j++)
                {
                    current = current.Next;
                }
                current.Value = value;
            }
        }

        private Node head = null;
        private int numElements = 0;

        public void Append(T value)
        {
            if (head == null)
            {
                head = new Node(value);
            }
            else
            {
                Node current = head;
                while (current.Next != null)
                {
                    current = current.Next;
                }
                current.Next = new Node(value);
            }
            numElements++;
        }

        public void Prepend(T value)
        {
            Node node = new Node(value, head);
            head = node;
            numElements++;
        }

        #region IEnumerator

        public IEnumerator<T> GetEnumerator()
        {
            Node current = head;
            while (current != null)
            {
                yield return current.Value;
                current = current.Next;
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            Node current = head;
            while (current != null)
            {
                yield return current.Value;
                current = current.Next;
            }
        }

        #endregion


        #region Node

        class Node
        {
            public T Value { get; set; }
            public Node Next { get; set; }

            public Node(T value, Node next = null)
            {
                Value = value;
                Next = next;
            }
        }

        #endregion

    }
}
