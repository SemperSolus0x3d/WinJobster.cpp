#include "Journal.h"

Journal::Journal() {}

Journal::~Journal()
{
    Rollback();
}

void Journal::Add(UndoCallback callback)
{
    m_Entries.push_front(callback);
}

void Journal::Commit()
{
    m_Entries.clear();
}

void Journal::Rollback()
{
    for (auto&& callback : m_Entries)
        callback();

    m_Entries.clear();
}

Journal& Journal::operator+=(UndoCallback callback)
{
    Add(callback);
    return *this;
}
