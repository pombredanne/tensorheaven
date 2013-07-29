// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/field.hpp by Victor Dods, created 2013/07/21
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_FIELD_HPP_
#define TENH_CONCEPTUAL_FIELD_HPP_

#include "tenh/core.hpp"

namespace Tenh {

// TODO: can Field_c be deprecated, in favor of just using IsAField_c with custom typedefs?
template <typename Id_>
struct Field_c
{
	typedef Id_ Id;

    static std::string type_as_string ()
    {
        return "Field_c<" + TypeStringOf_t<Id>::eval() + '>';
    }
};

template <typename T> struct IsAField_c { static bool const V = false; };
template <typename Id> struct IsAField_c<Field_c<Id> > { static bool const V = true; };

// // used for checking if a given C++ type is an acceptable representation for the field
// template <typename ScalarRepresentation, typename Field> struct IsAValidScalarRepresentationForField { static bool const V = false; };

// TODO: some way of specifying associated representation (e.g. double, complex<double>, bool)

struct Real_c
{
    static std::string type_as_string ()
    {
        return "Real_c";
    }
};

typedef Field_c<Real_c> RealField;
// template <> struct IsAValidScalarRepresentationForField<float,RealField> { static bool const V = true; };
// template <> struct IsAValidScalarRepresentationForField<double,RealField> { static bool const V = true; };
// // could add one for some arbitrary-precision floating point representation

// TODO: Complex_c, F2_c, etc.
// TODO: maybe quaternion (skew-field)?

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_FIELD_HPP_