/*
Beep Boop
January 2024
*/

#pragma once

#include <bitset>
#include <type_traits>

/* 
Class to make bitsets and enum classes play nice
1. Template class uses first template parameter "enumType" to get the types of the second parameter "enumSize"
2. Constructor has precompiler initialization ": bitsetHandle() {}" to 
3. create bitset of size enumType
4. Templates don't allow for source compatibility

FIXME(BB): review this code once we understand it better
Might want to use inheritance to and then override relevant methods
*/

template<typename enumType, enumType enumSize>
class EnumClassBitset
{
    public:
        EnumClassBitset(): bitsetHandle() {};

        bool test(enumType bit) const
        {
            return bitsetHandle.test(get_value(bit));
        }

        void set(enumType bit)
        {
            bitsetHandle.set(get_value(bit));
        }

        void reset(enumType bit)
        {
            bitsetHandle.reset(get_value(bit));
        }

        void operator|=(const EnumClassBitset& other)
        {
            bitsetHandle |= other.bitsetHandle;
        }

    private:
        std::bitset<static_cast<size_t>(enumSize)> bitsetHandle;

        typename std::underlying_type<enumType>::type get_value(enumType enumValue) const
        {
            return static_cast<typename std::underlying_type<enumType>::type>(enumValue);
        }
};