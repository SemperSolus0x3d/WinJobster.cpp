#pragma once
#include <functional>
#include <list>

class Journal
{
public:
    using UndoCallback = std::function<void()>;

    Journal();
    ~Journal();

    Journal(const Journal&) = delete;
    Journal(Journal&&) = delete;
    Journal& operator=(const Journal&) = delete;
    Journal& operator=(Journal&&) = delete;

    void Add(UndoCallback callback);
    void Commit();
    void Rollback();

    Journal& operator+=(UndoCallback callback);
private:
    std::list<UndoCallback> m_Entries;
};
