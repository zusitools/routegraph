#ifndef SHAREDDEQUE_H
#define SHAREDDEQUE_H

#include <QQueue>

/**
 * Provides a "shared double-ended queue" which contains a section of a shared "master queue".
 *
 * AB[CDEF]GHIJ
 *    end    start
 *   last     first
 *   at(0)    at(count)
 * dequeue  enqueue
 */
template<typename T>
class SharedDeque
{
public:
    SharedDeque<T>(QQueue<T> *masterQueue = NULL) :
        m_masterQueue(masterQueue),
        m_startIndex(-1),
        m_endIndex(0)
    {
    }

    /**
     * Copy constructor.
     */
    SharedDeque<T>(SharedDeque<T> *other)
    {
        this->m_startIndex = other->m_startIndex;
        this->m_endIndex = other->m_endIndex;
        this->m_masterQueue = other->m_masterQueue;
    }

    /**
     * Sets the master queue of this queue.
     */
    inline void setMasterQueue(QQueue<T>* masterQueue) {
        this->m_masterQueue = masterQueue;
    }

    /**
     * Returns whether the queue is empty.
     */
    inline bool isEmpty() const
    {
        return m_startIndex < m_endIndex;
    }

    /**
     * Returns whether the end of this queue corresponds to the end of the master queue.
     */
    inline bool atEnd() const
    {
        return m_startIndex == m_masterQueue->count() - 1;
    }

    /**
     * Returns the number of elements in the queue.
     */
    inline uint32_t count() const
    {
        return m_startIndex - m_endIndex + 1;
    }

    /**
     * Returns the element at the specified index.
     */
    inline T at(int32_t index) const
    {
        Q_ASSERT(m_endIndex + index <= m_masterQueue->count() - 1);
        return m_masterQueue->at(m_endIndex + index);
    }

    /**
     * Returns the last element in the queue.
     */
    inline T last() const
    {
        return at(0);
    }

    /**
     * Returns the first element in the queue.
     */
    inline T first() const {
        return m_masterQueue->at(m_startIndex);
    }

    /**
     * Enqueues the specified element. This only works when atEnd == true.
     */
    inline void enqueue(T element)
    {
        Q_ASSERT(m_startIndex == m_masterQueue->count() - 1);
        m_masterQueue->enqueue(element);
        m_startIndex++;
        Q_ASSERT(m_startIndex == m_masterQueue->count() - 1);
    }

    /**
     * Enqueues an element already existing in the master queue by incrementing the endIndex.
     * This only works when atEnd == false.
     */
    inline void enqueue()
    {
        Q_ASSERT(m_startIndex < m_masterQueue->count() - 1);
        m_startIndex++;
    }

    /**
     * Returns the last element in the queue and shortens the queue by one.
     */
    inline T dequeue()
    {
        Q_ASSERT(m_endIndex <= m_startIndex);
        return m_masterQueue->at(m_endIndex++);
    }

    /**
     * Returns the first element in the queue and shortens the queue by one from the front.
     */
    inline void dequeueFromStart()
    {
        Q_ASSERT(m_startIndex >= m_endIndex);
        m_startIndex--;
    }

    /**
     * Returns the index of the element in the queue or -1 if the element does not exist.
     * Search is done from the start towards the end.
     */
    inline int32_t lastIndexOf(T elem)
    {
        for (int i = count() - 1; i >= 0; i--) {
            if (at(i) == elem) {
                return i;
            }
        }

        return -1;
    }

    template<typename X>
    friend QDataStream &operator<< (QDataStream& stream, const SharedDeque<X> &deque);

    template<typename X>
    friend QDataStream &operator>> (QDataStream& stream, SharedDeque<X> &deque);

private:
    /**
     * The master queue from which to take the elements.
     */
    QQueue<T> *m_masterQueue;

    /**
     * The start index within the master queue.
     */
    int32_t m_startIndex;

    /**
     * The end index within the master queue.
     */
    int32_t m_endIndex;
};

/**
 * Saves information about a SharedDeque to a QDataStream.
 */
template<typename X>
QDataStream &operator<< (QDataStream& stream, const SharedDeque<X> &deque)
{
    stream << (qint32)deque.m_startIndex << (qint32)deque.m_endIndex;
    return stream;
}

/**
 * Loads information about a SharedDeque from a QDataStream.
 */
template<typename X>
QDataStream &operator>> (QDataStream& stream, SharedDeque<X>& deque)
{
    stream >> (qint32&)deque.m_startIndex >> (qint32&)deque.m_endIndex;
    return stream;
}

#endif // SHAREDDEQUE_H
