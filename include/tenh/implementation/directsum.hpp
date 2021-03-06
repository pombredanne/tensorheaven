// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/directsum.hpp by Ted Nitz, created 2013/10/24
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_DIRECT_SUM_HPP_
#define TENH_IMPLEMENTATION_DIRECT_SUM_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/diagonalbased2tensorproduct.hpp"
#include "tenh/conceptual/directsum.hpp"
#include "tenh/conceptual/scalarbased2tensorproduct.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/interface/vector.hpp"
#include "tenh/tuple.hpp"

namespace Tenh {

template <typename SummandTyple_, Uint32 N>
struct OffsetForComponent_f
{
private:
    static_assert(Length_f<SummandTyple_>::V > N, "attempted access past list end");
public:
    static const Uint32 V = DimensionOf_f<typename Head_f<SummandTyple_>::T>::V + OffsetForComponent_f<typename BodyTyple_f<SummandTyple_>::T, N-1>::V;
};

template <typename SummandTyple_>
struct OffsetForComponent_f<SummandTyple_,0>
{
    static const Uint32 V = 0;
};

inline Uint32 component_for_offset (Typle_t<> const &, Uint32 offset)
{
    assert(false && "this should never actually be called");
    return 0;
}

template <typename SummandTyple_>
Uint32 component_for_offset (SummandTyple_ const &, Uint32 offset)
{
    static_assert(Length_f<SummandTyple_>::V > 0, "SummandTyple_ length must be positive");
    typedef typename Head_f<SummandTyple_>::T HeadSummand;
    typedef typename BodyTyple_f<SummandTyple_>::T BodySummand;
    if (offset < DimensionOf_f<HeadSummand>::V)
        return 0;
    else
        return component_for_offset(BodySummand(), offset - DimensionOf_f<HeadSummand>::V);
}

template <typename SummandTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>
    :
    public Vector_i<typename DerivedType_f<Derived_,ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>>::T,
                    Scalar_,
                    DirectSumOfBasedVectorSpaces_c<SummandTyple_>,
                    ComponentQualifierOfArrayType_f<UseArrayType_>::V>,
    // privately inherited because it is an implementation detail
    private ArrayStorage_f<Scalar_,
                           DimensionOf_f<DirectSumOfBasedVectorSpaces_c<SummandTyple_>>::V,
                           UseArrayType_,
                           typename DerivedType_f<Derived_,ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>>::T >::T
{
    typedef Vector_i<typename DerivedType_f<Derived_,ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>>::T,
                     Scalar_,
                     DirectSumOfBasedVectorSpaces_c<SummandTyple_>,
                     ComponentQualifierOfArrayType_f<UseArrayType_>::V> Parent_Vector_i;
    typedef typename ArrayStorage_f<Scalar_,
                                    DimensionOf_f<DirectSumOfBasedVectorSpaces_c<SummandTyple_>>::V,
                                    UseArrayType_,
                                    typename DerivedType_f<Derived_,ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>>::T >::T Parent_Array_i;

    typedef DirectSumOfBasedVectorSpaces_c<SummandTyple_> Concept;
    typedef UseArrayType_ UseArrayType;
    typedef typename Parent_Vector_i::Derived Derived;
    typedef typename Parent_Vector_i::Scalar Scalar;
    using Parent_Vector_i::DIM;
    typedef typename Parent_Vector_i::ComponentIndex ComponentIndex;
    typedef typename Parent_Vector_i::MultiIndex MultiIndex;

    using Parent_Array_i::COMPONENT_QUALIFIER;
    typedef typename Parent_Array_i::ComponentAccessConstReturnType ComponentAccessConstReturnType;
    typedef typename Parent_Array_i::ComponentAccessNonConstReturnType ComponentAccessNonConstReturnType;
    typedef typename Parent_Array_i::QualifiedComponent QualifiedComponent;

    typedef typename DualOf_f<ImplementationOf_t>::T Dual; // relies on the template specialization below

    typedef ImplementationOf_t<Concept,
                               Scalar_,
                               UseProceduralArray_t<typename ComponentGenerator_Constant_f<Scalar_,DIM,0>::T>,
                               Derived_> Zero;
    static Zero const ZERO;

    template <Uint32 INDEX_>
    struct BasisVector_f
    {
        static_assert(INDEX_ < Parent_Vector_i::DIM, "index out of range");
        typedef ImplementationOf_t<Concept,
                                   Scalar_,
                                   UseProceduralArray_t<typename ComponentGenerator_Characteristic_f<Scalar_,Parent_Vector_i::DIM,INDEX_>::T>,
                                   Derived_> T;
        static T const V;
    private:
        BasisVector_f () { }
    };

    explicit ImplementationOf_t (WithoutInitialization const &w) : Parent_Array_i(w) { }

    // only use these if UseMemberArray_t<...> is specified

    // similar to a copy constructor, except initializes from a Vector_i.
    // this was chosen to be explicit to avoid unnecessary copies.
    template <typename OtherDerived_, ComponentQualifier OTHER_COMPONENT_QUALIFIER_>
    explicit ImplementationOf_t (Vector_i<OtherDerived_,Scalar_,Concept,OTHER_COMPONENT_QUALIFIER_> const &x)
        :
        Parent_Array_i(Static<WithoutInitialization>::SINGLETON)
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
        // TODO: could make this use MemoryArray_i::copy_from (?)
        for (ComponentIndex i; i.is_not_at_end(); ++i)
            (*this)[i] = x[i];
    }
    // probably only useful for zero element (because this is basis-dependent)
    template <typename T_>
    explicit ImplementationOf_t (FillWith_t<T_> const &fill_with)
        :
        Parent_Array_i(fill_with)
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
    }
    // this is the tuple-based constructor
    template <typename... Types_>
    ImplementationOf_t (Tuple_t<Typle_t<Types_...>> const &x)
        :
        Parent_Array_i(x.as_member_array())
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
    }

    // only use these if UsePreallocatedArray_t<...> is specified

    explicit ImplementationOf_t (QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }
    // similar to a copy constructor, except initializes from a Vector_i.
    // this was chosen to be explicit to avoid unnecessary copies.
    template <typename OtherDerived_, ComponentQualifier OTHER_COMPONENT_QUALIFIER_>
    ImplementationOf_t (Vector_i<OtherDerived_,Scalar_,Concept,OTHER_COMPONENT_QUALIFIER_> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
        // TODO: could make this use MemoryArray_i::copy_from (?)
        for (ComponentIndex i; i.is_not_at_end(); ++i)
            (*this)[i] = x[i];
    }
    template <typename T_>
    ImplementationOf_t (FillWith_t<T_> const &fill_with,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(fill_with, pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }
    // this is the tuple-based constructor
    template <typename... Types_>
    ImplementationOf_t (Tuple_t<Typle_t<Types_...>> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(x, pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }

    // only use this if UseProceduralArray_t<...> is specified or if the vector space is 0-dimensional
    ImplementationOf_t ()
        :
        Parent_Array_i(WithoutInitialization()) // sort of meaningless constructor
    {
        static_assert(IsUseProceduralArray_f<UseArrayType_>::V || DIM == 0, "UseArrayType_ must be UseProceduralArray_t or space must be 0-dimensional");
    }

    using Parent_Array_i::as_derived;
    using Parent_Array_i::operator[];
    using Parent_Array_i::allocation_size_in_bytes;
    using Parent_Array_i::pointer_to_allocation;
    using Parent_Array_i::overlaps_memory_range;

    template <Uint32 N_, ForceConst FORCE_CONST_>
    struct ElementType_f
    {
        static_assert(Length_f<SummandTyple_>::V > N_, "attempted access past list end");
    private:
        static ComponentsAreConst const ELEMENT_COMPONENTS_ARE_CONST = 
            ComponentsAreConst(bool(FORCE_CONST_) || ComponentQualifierOfArrayType_f<UseArrayType_>::V == ComponentQualifier::CONST_MEMORY);
    public:
        typedef ImplementationOf_t<typename Element_f<SummandTyple_,N_>::T,
                                   Scalar_,
                                   typename If_f<IsUseProceduralArray_f<UseArrayType_>::V,
                                                 UseArrayType_,
                                                 UsePreallocatedArray_t<ELEMENT_COMPONENTS_ARE_CONST>>::T > T;
    };

    template <Uint32 N_>
    typename ElementType_f<N_,ForceConst::FALSE>::T el ()
    {
        static_assert(Length_f<SummandTyple_>::V > N_, "attempted access past list end");
        return typename ElementType_f<N_,ForceConst::FALSE>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTyple_,N_>::V);
    }

    template <Uint32 N_>
    typename ElementType_f<N_,ForceConst::TRUE>::T el () const
    {
        static_assert(Length_f<SummandTyple_>::V > N_, "attempted access past list end");
        return typename ElementType_f<N_,ForceConst::TRUE>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTyple_,N_>::V);
    }

    typename ElementType_f<0,ForceConst::FALSE>::T el (Uint32 n)
    {
        static_assert(TypleIsUniform_f<SummandTyple_>::V, "SummandTyple_ must be uniform");
        return typename ElementType_f<0,ForceConst::FALSE>::T(pointer_to_allocation() + DimensionOf_f<typename Head_f<SummandTyple_>::T>::V * n);
    }

    typename ElementType_f<0,ForceConst::TRUE>::T el (Uint32 n) const
    {
        static_assert(TypleIsUniform_f<SummandTyple_>::V, "SummandTyple_ must be uniform");
        return typename ElementType_f<0,ForceConst::TRUE>::T(pointer_to_allocation() + DimensionOf_f<typename Head_f<SummandTyple_>::T>::V * n);
    }

    // These versions of el<...> are intended to allow use like el<n>(i) rather than the more clunky el<n>()(i) to get an indexed expression.
    // Unfortunately the indexed expression type stores a reference to the ImplementationOf_t internally, and in this code that object is
    // a temporary, and so the behavior is undefined.
    // template <Uint32 N, AbstractIndexSymbol SYMBOL_>
    // typename ElementType_f<N>::T::template IndexedExpressionNonConstType_f<SYMBOL_>::T el(AbstractIndex_c<SYMBOL_> const & i)
    // {
    //     static_assert(SummandTyple_::LENGTH > N, "attempted access past list end");
    //     return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTyple_,N>::V)(i);
    // }
    //
    // template <Uint32 N, AbstractIndexSymbol SYMBOL_>
    // typename ElementType_f<N>::T::template IndexedExpressionConstType_f<SYMBOL_>::T el(AbstractIndex_c<SYMBOL_> const & i) const
    // {
    //     static_assert(SummandTyple_::LENGTH > N, "attempted access past list end");
    //     return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTyple_,N>::V)(i);
    // }
};

template <typename SummandTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
typename ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>::Zero const ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>::ZERO;

template <typename SummandTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <Uint32 INDEX_>
typename ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>::template BasisVector_f<INDEX_>::T const ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>::BasisVector_f<INDEX_>::V;

template <typename SummandTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct DualOf_f<ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTyple_>,Scalar_,UseArrayType_,Derived_>>
{
    typedef ImplementationOf_t<typename DualOf_f<DirectSumOfBasedVectorSpaces_c<SummandTyple_>>::T,Scalar_,typename DualOf_f<UseArrayType_>::T, typename DualOf_f<Derived_>::T> T;
};

// ///////////////////////////////////////////////////////////////////////////
// direct sum of procedural 2-tensors (essentially gives a block-diag matrix)
// ///////////////////////////////////////////////////////////////////////////

template <typename Procedural2TensorImplementationTyple_>
struct ConceptualTypeOfDirectSumOfProcedural2Tensors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTyple_>::T ConceptTyple;
    static_assert(EachTypeSatisfies_f<ConceptTyple,IsTensorProductOfBasedVectorSpaces_e>::V
                  ||
                  EachTypeSatisfies_f<ConceptTyple,IsDiagonal2TensorProductOfBasedVectorSpaces_e>::V
                  ||
                  EachTypeSatisfies_f<ConceptTyple,IsScalar2TensorProductOfBasedVectorSpaces_e>::V,
                  "must be a typle of scalar or diagonal or 2-tensors");
    typedef typename FactorNOfEachTypeIn_f<0,ConceptTyple>::T SummandTyple0;
    typedef typename FactorNOfEachTypeIn_f<1,ConceptTyple>::T SummandTyple1;
    typedef DirectSumOfBasedVectorSpaces_c<SummandTyple0> Factor0DirectSum;
    typedef DirectSumOfBasedVectorSpaces_c<SummandTyple1> Factor1DirectSum;
    ConceptualTypeOfDirectSumOfProcedural2Tensors_f();
public:
    typedef TensorProductOfBasedVectorSpaces_c<Typle_t<Factor0DirectSum,Factor1DirectSum>> T;
};

namespace ComponentGeneratorEvaluator {

template <typename Procedural2TensorImplementationTyple_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
struct DirectSumOf2TensorsHelper_t
{
    typedef ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> ComponentIndex;
    typedef typename ConceptualTypeOfDirectSumOfProcedural2Tensors_f<Procedural2TensorImplementationTyple_>::T ConceptualTypeOfDirectSum;
    static_assert(TypesAreEqual_f<ConceptualTypeOfDirectSum_,ConceptualTypeOfDirectSum>::V, "types must be equal");
    typedef typename FactorTypleOf_f<ConceptualTypeOfDirectSum_>::T FactorTyple;
    typedef typename Element_f<FactorTyple,0>::T Factor0;
    typedef typename Element_f<FactorTyple,1>::T Factor1;
    typedef typename ImplementationOf_t<ConceptualTypeOfDirectSum_,Scalar_,UseMemberArray_t<ComponentsAreConst::FALSE>>::MultiIndex MultiIndex;
    static_assert(MultiIndex::LENGTH == 2, "length must be exactly 2");
    typedef typename Head_f<Procedural2TensorImplementationTyple_>::T HeadImplementation;
    typedef typename FactorTypleOf_f<typename HeadImplementation::Concept>::T HeadFactorTyple;
    typedef typename Element_f<HeadFactorTyple,0>::T HeadFactor0;
    typedef typename Element_f<HeadFactorTyple,1>::T HeadFactor1;

    static Scalar_ evaluate (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
    {
        // this breaks up the component index into the corresponding multiindex.
        MultiIndex m(i);
        bool first_block_for_row = m.template el<0>().value() < DimensionOf_f<HeadFactor0>::V;
        bool first_block_for_col = m.template el<1>().value() < DimensionOf_f<HeadFactor1>::V;

        if (first_block_for_row != first_block_for_col) // off block-diagonal
        {
            return Scalar_(0);
        }
        else if (first_block_for_row && first_block_for_col) // on block-diagonal, upper-left block
        {
            AbstractIndex_c<'j'> j;
            AbstractIndex_c<'k'> k;
            HeadImplementation h;
            typedef typename HeadImplementation::MultiIndex M;
            M head_m(m.template el<0>().value(), m.template el<1>().value(), CheckRange::FALSE);
            // this split is unnecessary if HeadProcedural2TensorImplementation_ is a tensor product,
            // but this makes the same code work for diagonal 2 tensors as well, so for now that's fine.
            return h.split(j*k)[head_m];
        }
        else // body block
        {
            typedef typename BodyTyple_f<Procedural2TensorImplementationTyple_>::T Procedural2TensorImplementationBodyTyple;
            typedef typename ConceptualTypeOfDirectSumOfProcedural2Tensors_f<Procedural2TensorImplementationBodyTyple>::T ConceptualTypeOfDirectSumBody;
            typedef DirectSumOf2TensorsHelper_t<Procedural2TensorImplementationBodyTyple,
                                                ConceptualTypeOfDirectSumBody,
                                                Scalar_> DirectSumOf2TensorsHelper;
            typedef typename DirectSumOf2TensorsHelper::MultiIndex BodyMultiIndex;
            BodyMultiIndex body_m(m.template el<0>().value() - DimensionOf_f<HeadFactor0>::V,
                                  m.template el<1>().value() - DimensionOf_f<HeadFactor1>::V,
                                  CheckRange::FALSE);
            return DirectSumOf2TensorsHelper::evaluate(body_m.as_component_index());
        }
    }
};

template <typename HeadProcedural2TensorImplementation_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
struct DirectSumOf2TensorsHelper_t<Typle_t<HeadProcedural2TensorImplementation_>,ConceptualTypeOfDirectSum_,Scalar_>
{
    typedef typename HeadProcedural2TensorImplementation_::MultiIndex MultiIndex;

    static Scalar_ evaluate (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
    {
        // this breaks up the component index into the corresponding multiindex.
        typename HeadProcedural2TensorImplementation_::MultiIndex m(i);
        HeadProcedural2TensorImplementation_ h;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        // this split is unnecessary if HeadProcedural2TensorImplementation_ is a tensor product,
        // but this makes the same code work for diagonal 2 tensors as well, so for now that's fine.
        return h.split(j*k)[m];
    }
};

template <typename Procedural2TensorImplementationTyple_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
Scalar_ direct_sum_of_2tensors (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
{
    return DirectSumOf2TensorsHelper_t<Procedural2TensorImplementationTyple_,
                                       ConceptualTypeOfDirectSum_,
                                       Scalar_>::evaluate(i);
}

} // end of namespace ComponentGeneratorEvaluator

template <typename Procedural2TensorImplementationTyple_>
struct DirectSumOfProcedural2Tensors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTyple_>::T ConceptTyple;
    typedef typename ScalarOfEachTypeIn_f<Procedural2TensorImplementationTyple_>::T ScalarTyple;
    typedef typename ConceptualTypeOfDirectSumOfProcedural2Tensors_f<Procedural2TensorImplementationTyple_>::T ConceptualTypeOfDirectSum;

    static_assert(TypleIsUniform_f<ScalarTyple>::V, "all factor type scalars must be equal");
    static_assert(EachTypeUsesProceduralArray_f<Procedural2TensorImplementationTyple_>::V, "must be a typle of procedural implementations");

    typedef typename Head_f<ScalarTyple>::T Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<ConceptualTypeOfDirectSum>::V,
                                 ComponentGeneratorEvaluator::direct_sum_of_2tensors<Procedural2TensorImplementationTyple_,
                                                                                     ConceptualTypeOfDirectSum,
                                                                                     Scalar>,
                                 DirectSum_c<Procedural2TensorImplementationTyple_>> ComponentGenerator;
private:
    DirectSumOfProcedural2Tensors_f();
public:
    typedef ImplementationOf_t<ConceptualTypeOfDirectSum,Scalar,UseProceduralArray_t<ComponentGenerator>> T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_DIRECT_SUM_HPP_
