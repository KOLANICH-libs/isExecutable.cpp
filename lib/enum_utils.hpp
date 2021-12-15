#include <type_traits>
#include <compare>

template<typename EnumType>
constexpr std::enable_if_t<std::is_enum<EnumType>::value, EnumType> operator&(const EnumType a, const EnumType b){
	using UT = std::underlying_type_t<EnumType>;
	return static_cast<EnumType>(static_cast<UT>(a) & static_cast<UT>(b));
};

template<typename EnumType, typename IntType>
constexpr std::enable_if_t<std::is_enum<EnumType>::value && std::is_integral<IntType>::value, EnumType> operator+(const EnumType a, const IntType b){
	using UT = std::underlying_type_t<EnumType>;
	return static_cast<EnumType>(static_cast<UT>(a) + b);
};

template<typename EnumType>
constexpr std::enable_if_t<std::is_enum<EnumType>::value, EnumType> operator++(const EnumType a){
	using UT = std::underlying_type_t<EnumType>;
	return static_cast<EnumType>(static_cast<UT>(a) + 1);
};

template<typename EnumType, typename IntType>
constexpr std::enable_if_t<std::is_enum<EnumType>::value && std::is_integral<IntType>::value, std::strong_ordering> operator<=>(const EnumType a, const IntType b){
	using UT = std::underlying_type_t<EnumType>;
	return static_cast<UT>(a) <=> b;
};


template<typename EnumType>
constexpr std::enable_if_t<std::is_enum<EnumType>::value, bool> check_flag(EnumType flag, EnumType v){
	return (flag & v) == flag;
};
