/*=============================================================================
 * TarotClub - UniqueId.h
 *=============================================================================
 * Unique User identifier utility class
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <cstdint>
#include <list>
#include <mutex>

/*****************************************************************************/
/**
 * @brief The UniqueId class
 *
 * Manage a unique list of IDs.
 * Start at 1, 0 is considered as invalid
 *
 */
class UniqueId
{
public:
    static const std::uint32_t cInvalidId = 0U;

    UniqueId(std::uint32_t min, std::uint32_t max);

    std::uint32_t TakeId();
    std::uint32_t FindId();
    void AddId(std::uint32_t id);
    bool ReleaseId(std::uint32_t id);
    bool IsTaken(std::uint32_t id);
    std::uint32_t GetMin() { return mMin; }
    std::uint32_t GetMax() { return mMax; }
    void Clear() { mUsedIds.clear(); }
    void Dump();

private:
    std::uint32_t mMin;
    std::uint32_t mMax;
    std::list<std::uint32_t> mUsedIds;
};

#endif // UNIQUE_ID_H

//=============================================================================
// End of file UniqueId.h
//=============================================================================
