#ifndef EXPRESSION_TEMPLATES_HPP_
#define EXPRESSION_TEMPLATES_HPP_

#include "core.hpp"
#include "compoundindex.hpp"
#include "naturalpairing.hpp"
#include "typelist.hpp"

// this is essentially a compile-time interface, requiring:
// - a Derived type (should be the type of the thing that ultimately inherits this)
// - a Scalar type (should be the scalar type of the expression template's tensor operand)
// - a FreeIndexTypeList type (the free indices of this expression template)
// - a UsedIndexTypeList type (the indices that have been used further down the AST)
// requires also particular methods:
//   operator Scalar () const // conversion to scalar -- always declared, but should 
//                            // only compile if the conversion is well-defined (e.g. no free indices)
//   Scalar operator [] (CompoundIndex const &) const // accessor using CompoundIndex_t<FreeIndexTypeList>
//   template <typename OtherTensor> bool uses_tensor (OtherTensor const &) const // used in checking for aliasing
template <typename Derived_, typename Scalar_, typename FreeIndexTypeList_, typename UsedIndexTypeList_>
struct ExpressionTemplate_i // _i is for "compile-time interface"
{
    typedef Derived_ Derived;
    // these typedefs make the Derived-specified typedefs available at the baseclass level,
    typedef Scalar_ Scalar;
    typedef FreeIndexTypeList_ FreeIndexTypeList;
    typedef UsedIndexTypeList_ UsedIndexTypeList;
    typedef CompoundIndex_t<FreeIndexTypeList> CompoundIndex;
    static bool const IS_EXPRESSION_TEMPLATE = true;
 
    // TODO: some consistency checks on the various types

    // compile-time interface methods    
    operator Scalar () const { return as_derived().operator Scalar(); }
    Scalar operator [] (CompoundIndex const &c) const { return as_derived().operator[](c); }
    template <typename OtherTensor> bool uses_tensor (OtherTensor const &t) const { return as_derived().template uses_tensor<OtherTensor>(t); }

    // for accessing this as the Derived type
    Derived const &as_derived () const { return *static_cast<Derived const *>(this); }
    Derived &as_derived () { return *static_cast<Derived *>(this); }
};

// ////////////////////////////////////////////////////////////////////////////
// summation over repeated indices
// ////////////////////////////////////////////////////////////////////////////

template <typename IndexTypeList>
struct SummedIndexTypeList_t
{
    typedef typename ElementsHavingMultiplicity_t<IndexTypeList,2>::T T;
};

template <typename IndexTypeList>
struct FreeIndexTypeList_t
{
    typedef typename ElementsHavingMultiplicity_t<IndexTypeList,1>::T T;
};

template <typename HeadType>
typename HeadType::OwnerVector::Scalar summation_component_factor (CompoundIndex_t<TypeList_t<HeadType> > const &s)
{
    return NaturalPairing_t<typename HeadType::OwnerVector>::component(s.head());
}

template <typename HeadType, typename BodyTypeList>
typename HeadType::OwnerVector::Scalar summation_component_factor (CompoundIndex_t<TypeList_t<HeadType,BodyTypeList> > const &s)
{
    return NaturalPairing_t<typename HeadType::OwnerVector>::component(s.head()) * summation_component_factor(s.body());
}

// TODO: think about how UnarySummation_t and BinarySummation_t could be combined (if it makes sense to do it)

// this is designed to handle trace-type expression templates, such as u(i,i) or v(i,j,i)
// technically SummedIndexTypeList is a redundant argument (as it is derivable from TensorIndexTypeList),
// but it is necessary so that a template specialization can be made for when it is EmptyTypeList.
template <typename Tensor, typename TensorIndexTypeList, typename SummedIndexTypeList>
struct UnarySummation_t
{
    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTypeList_t<TensorIndexTypeList>::T FreeIndexTypeList;
    typedef CompoundIndex_t<FreeIndexTypeList> CompoundIndex;

    static Scalar eval (Tensor const &tensor, CompoundIndex const &c)
    {
        typedef typename ConcatenationOfTypeLists_t<FreeIndexTypeList,SummedIndexTypeList>::T TotalIndexTypeList;
        typedef CompoundIndex_t<TotalIndexTypeList> TotalIndex;
        typedef CompoundIndex_t<SummedIndexTypeList> SummedIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with c initializes the first elements which correpond to
        // CompoundIndex with the value of c, and initializes the remaining elements to zero.
        TotalIndex t(c);
        Scalar retval(0);
        // get the map which produces the CompoundIndex for each tensor from the TotalIndex t
        typedef CompoundIndexMap_t<TotalIndexTypeList,TensorIndexTypeList> TensorIndexMap;
        typename TensorIndexMap::EvalMapType tensor_index_map = TensorIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedIndex &s = t.template trailing_list<FreeIndexTypeList::LENGTH>(); s.is_not_at_end(); ++s)
            retval += tensor[tensor_index_map(t)] * summation_component_factor(s);
        return retval;
    }
};

template <typename Tensor, typename TensorIndexTypeList>
struct UnarySummation_t<Tensor,TensorIndexTypeList,EmptyTypeList>
{
    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTypeList_t<TensorIndexTypeList>::T FreeIndexTypeList;
    typedef CompoundIndex_t<FreeIndexTypeList> CompoundIndex;

    static Scalar eval (Tensor const &tensor, CompoundIndex const &c) { return tensor[c]; }
};

// ////////////////////////////////////////////////////////////////////////////
// expression-template-generation (making ETs from vectors/tensors)
// ////////////////////////////////////////////////////////////////////////////

// this is the "const" version of an indexed tensor expression (it has summed indices, so it doesn't make sense to assign to it)
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename Tensor, typename TensorIndexTypeList, typename SummedIndexTypeList>
struct ExpressionTemplate_IndexedTensor_t 
    : 
    public ExpressionTemplate_i<ExpressionTemplate_IndexedTensor_t<Tensor,TensorIndexTypeList,SummedIndexTypeList>,
                                typename Tensor::Scalar,
                                typename FreeIndexTypeList_t<TensorIndexTypeList>::T,
                                SummedIndexTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_IndexedTensor_t<Tensor,TensorIndexTypeList,SummedIndexTypeList>,
                                 typename Tensor::Scalar,
                                 typename FreeIndexTypeList_t<TensorIndexTypeList>::T,
                                 SummedIndexTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::CompoundIndex CompoundIndex;
    using Parent::IS_EXPRESSION_TEMPLATE;
    
    ExpressionTemplate_IndexedTensor_t (Tensor const &tensor) : m_tensor(tensor) { }

    operator Scalar () const
    {
        Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<FreeIndexTypeList,EmptyTypeList>::v>();
        return operator[](CompoundIndex());
    }
    
    // read-only, because it doesn't make sense to assign to an expression which is a summation.
    Scalar operator [] (CompoundIndex const &c) const
    {
        return UnarySummation_t<Tensor,TensorIndexTypeList,SummedIndexTypeList>::eval(m_tensor, c);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        // the reinterpret_cast is safe because we're dealing with POD types and there
        // is an explicit type-check at compiletime (TypesAreEqual)
        return Lvd::Meta::TypesAreEqual<OtherTensor,Tensor>::v && reinterpret_cast<Tensor const *>(&t) == &m_tensor;
    }

private:

    Tensor const &m_tensor;
};

// this is the "non-const" version of an indexed tensor expression (it has no summed indices, so it makes sense to assign to it)
template <typename Tensor, typename TensorIndexTypeList>
struct ExpressionTemplate_IndexedTensor_t<Tensor,TensorIndexTypeList,EmptyTypeList>
    : 
    public ExpressionTemplate_i<ExpressionTemplate_IndexedTensor_t<Tensor,TensorIndexTypeList,EmptyTypeList>,
                                typename Tensor::Scalar,
                                typename FreeIndexTypeList_t<TensorIndexTypeList>::T,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_IndexedTensor_t<Tensor,TensorIndexTypeList,EmptyTypeList>,
                                 typename Tensor::Scalar,
                                 typename FreeIndexTypeList_t<TensorIndexTypeList>::T,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::CompoundIndex CompoundIndex;
    using Parent::IS_EXPRESSION_TEMPLATE;

    ExpressionTemplate_IndexedTensor_t (Tensor &tensor) : m_tensor(tensor) { }

    operator Scalar () const
    {
        Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<FreeIndexTypeList,EmptyTypeList>::v>();
        return operator[](CompoundIndex());
    }
    
    // read-only, because it doesn't necessarily make sense to assign to an expression
    // template -- the expression may be a product or some such, where each component
    // is not an L-value.
    Scalar const &operator [] (typename Tensor::Index const &i) const { return m_tensor[i]; }
    Scalar operator [] (CompoundIndex const &c) const { return m_tensor[c]; }

    // for some dumb reason, the compiler needed a non-templatized assignment operator for the exact matching type
    void operator = (ExpressionTemplate_IndexedTensor_t const &right_operand)
    {
        // if right and left operands' m_tensor references are the same, this is a no-op
        if (&right_operand.m_tensor == &m_tensor)
            return;

        // TODO: replace with memcpy? (this would require that Scalar is a POD type)
        for (typename Tensor::Index i; i.is_not_at_end(); ++i)
            m_tensor[i] = right_operand[i];
    }
    template <typename RightOperand>
    void operator = (RightOperand const &right_operand)
    {
        enum
        {
            RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE        = Lvd::Meta::Assert<RightOperand::IS_EXPRESSION_TEMPLATE>::v,
            OPERAND_SCALAR_TYPES_ARE_EQUAL              = Lvd::Meta::Assert<(Lvd::Meta::TypesAreEqual<Scalar,typename RightOperand::Scalar>::v)>::v,
            OPERANDS_HAVE_SAME_FREE_INDICES             = Lvd::Meta::Assert<AreEqualAsSets_t<FreeIndexTypeList,typename RightOperand::FreeIndexTypeList>::V>::v,
            LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES  = Lvd::Meta::Assert<!ContainsDuplicates_t<FreeIndexTypeList>::V>::v,
            RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES = Lvd::Meta::Assert<!ContainsDuplicates_t<typename RightOperand::FreeIndexTypeList>::V>::v
        };

        // check for aliasing (where source and destination memory overlap)
        if (right_operand.uses_tensor(m_tensor))
            throw std::invalid_argument("invalid aliased tensor assignment (source and destination memory overlap) -- use an intermediate value");

        typedef CompoundIndexMap_t<FreeIndexTypeList,typename RightOperand::FreeIndexTypeList> RightOperandIndexMap;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;

        // component-wise assignment via the free index type.
        for (CompoundIndex c; c.is_not_at_end(); ++c)
            m_tensor[c] = right_operand[right_operand_index_map(c)];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        // the reinterpret_cast is safe because we're dealing with POD types and there
        // is an explicit type-check at compiletime (TypesAreEqual)
        return Lvd::Meta::TypesAreEqual<OtherTensor,Tensor>::v && reinterpret_cast<Tensor const *>(&t) == &m_tensor;
    }

private:

    Tensor &m_tensor;
};

// ////////////////////////////////////////////////////////////////////////////
// addition of expression templates
// ////////////////////////////////////////////////////////////////////////////

// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand, char OPERATOR>
struct ExpressionTemplate_Addition_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                typename LeftOperand::Scalar,
                                typename LeftOperand::FreeIndexTypeList,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                 typename LeftOperand::Scalar,
                                 typename LeftOperand::FreeIndexTypeList,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::CompoundIndex CompoundIndex;
    using Parent::IS_EXPRESSION_TEMPLATE;
                                 
    // TODO: check that the summed indices from each operand have no indices in common
    // though technically this is unnecessary, because the summed indices are "private"
    // to each contraction, so this is really for the human's benefit, not getting
    // confused by multiple repeated indices that have nothing to do with each other.
    // NOTE: technically this check is already done inside CompoundIndex_t, but it would
    // be good to do the check here so that an error will be more obvious.
    enum
    {
        LEFT_OPERAND_IS_EXPRESSION_TEMPLATE         = Lvd::Meta::Assert<LeftOperand::IS_EXPRESSION_TEMPLATE>::v,
        RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE        = Lvd::Meta::Assert<RightOperand::IS_EXPRESSION_TEMPLATE>::v,
        OPERAND_SCALAR_TYPES_ARE_EQUAL              = Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v>::v,
        OPERANDS_HAVE_SAME_FREE_INDICES             = Lvd::Meta::Assert<AreEqualAsSets_t<typename LeftOperand::FreeIndexTypeList,typename RightOperand::FreeIndexTypeList>::V>::v,
        LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES  = Lvd::Meta::Assert<!ContainsDuplicates_t<typename LeftOperand::FreeIndexTypeList>::V>::v,
        RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES = Lvd::Meta::Assert<!ContainsDuplicates_t<typename RightOperand::FreeIndexTypeList>::V>::v,
        OPERATOR_IS_PLUS_OR_MINUS                   = Lvd::Meta::Assert<(OPERATOR == '+' || OPERATOR == '-')>::v
    };

    ExpressionTemplate_Addition_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    operator Scalar () const
    {
        Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<FreeIndexTypeList,EmptyTypeList>::v>();
        if (OPERATOR == '+')
            return m_left_operand.operator Scalar() + m_right_operand.operator Scalar();
        else // OPERATOR == '-'
            return m_left_operand.operator Scalar() - m_right_operand.operator Scalar();
    }

    Scalar operator [] (CompoundIndex const &c) const
    {
        typedef CompoundIndexMap_t<FreeIndexTypeList,typename LeftOperand::FreeIndexTypeList> LeftOperandIndexMap;
        typedef CompoundIndexMap_t<FreeIndexTypeList,typename RightOperand::FreeIndexTypeList> RightOperandIndexMap;
        typename LeftOperandIndexMap::EvalMapType left_operand_index_map = LeftOperandIndexMap::eval;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;
        if (OPERATOR == '+')
            return m_left_operand[left_operand_index_map(c)] + m_right_operand[right_operand_index_map(c)];
        else // OPERATOR == '-'
            return m_left_operand[left_operand_index_map(c)] - m_right_operand[right_operand_index_map(c)];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_left_operand.uses_tensor(t) || m_right_operand.uses_tensor(t);
    }

private:

    LeftOperand const &m_left_operand;
    RightOperand const &m_right_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// scalar multiplication and division of expression templates
// ////////////////////////////////////////////////////////////////////////////

// it is assumed that scalar multiplication is commutative.
// OPERATOR can be '*' or '/'.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename Operand, typename Scalar_, char OPERATOR>
struct ExpressionTemplate_ScalarMultiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                typename Operand::Scalar,
                                typename Operand::FreeIndexTypeList,
                                typename Operand::UsedIndexTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                 typename Operand::Scalar,
                                 typename Operand::FreeIndexTypeList,
                                 typename Operand::UsedIndexTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::CompoundIndex CompoundIndex;
    using Parent::IS_EXPRESSION_TEMPLATE;
    
    enum
    {
        OPERAND_IS_EXPRESSION_TEMPLATE = Lvd::Meta::Assert<Operand::IS_EXPRESSION_TEMPLATE>::v,
        OPERAND_SCALAR_MATCHES_SCALAR  = Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename Operand::Scalar,Scalar_>::v>::v,
        OPERATOR_IS_VALID              = Lvd::Meta::Assert<(OPERATOR == '*' || OPERATOR == '/')>::v
    };
    
    ExpressionTemplate_ScalarMultiplication_t (Operand const &operand, Scalar scalar_operand)
        :
        m_operand(operand),
        m_scalar_operand(scalar_operand)
    { 
        // TODO: should there be a runtime check here if OPERATOR is '/' and scalar_operand is zero (or close to zero)?
    }
    
    operator Scalar () const
    {
        Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<FreeIndexTypeList,EmptyTypeList>::v>();
        return operator[](CompoundIndex());
    }

    Scalar operator [] (CompoundIndex const &c) const
    {
        if (OPERATOR == '*')
            return m_operand[c] * m_scalar_operand;
        else
            return m_operand[c] / m_scalar_operand;
    }
    
    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_operand.uses_tensor(t);
    }

private:

    Operand const &m_operand;
    Scalar m_scalar_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// multiplication of expression templates (tensor product and contraction)
// ////////////////////////////////////////////////////////////////////////////

template <typename LeftOperand, typename RightOperand, typename FreeIndexTypeList, typename SummedIndexTypeList>
struct BinarySummation_t
{
    enum { _ = Lvd::Meta::Assert<LeftOperand::IS_EXPRESSION_TEMPLATE>::v &&
               Lvd::Meta::Assert<RightOperand::IS_EXPRESSION_TEMPLATE>::v &&
               Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v>::v &&
               Lvd::Meta::Assert<(SummedIndexTypeList::LENGTH > 0)>::v };

    typedef typename LeftOperand::Scalar Scalar;
    typedef CompoundIndex_t<FreeIndexTypeList> CompoundIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, CompoundIndex const &c)
    {
        typedef typename ConcatenationOfTypeLists_t<FreeIndexTypeList,SummedIndexTypeList>::T TotalIndexTypeList;
        typedef CompoundIndex_t<TotalIndexTypeList> TotalIndex;
        typedef CompoundIndex_t<SummedIndexTypeList> SummedIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with c initializes the first elements which correpond to
        // CompoundIndex with the value of c, and initializes the remaining elements to zero.
        TotalIndex t(c);
        Scalar retval(0);
        // get the map which produces the CompoundIndex for each operand from the TotalIndex t
        typedef CompoundIndexMap_t<TotalIndexTypeList,typename LeftOperand::FreeIndexTypeList> LeftOperandIndexMap;
        typedef CompoundIndexMap_t<TotalIndexTypeList,typename RightOperand::FreeIndexTypeList> RightOperandIndexMap;
        typename LeftOperandIndexMap::EvalMapType left_operand_index_map = LeftOperandIndexMap::eval;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedIndex &s = t.template trailing_list<FreeIndexTypeList::LENGTH>(); s.is_not_at_end(); ++s)
            retval += left_operand[left_operand_index_map(t)] *
                      right_operand[right_operand_index_map(t)] *
                      summation_component_factor(s);
        return retval;
    }
};

// template specialization handles summation over no indices
template <typename LeftOperand, typename RightOperand, typename FreeIndexTypeList>
struct BinarySummation_t<LeftOperand,RightOperand,FreeIndexTypeList,EmptyTypeList>
{
    enum { _ = Lvd::Meta::Assert<LeftOperand::IS_EXPRESSION_TEMPLATE>::v &&
               Lvd::Meta::Assert<RightOperand::IS_EXPRESSION_TEMPLATE>::v &&
               Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v>::v };

    typedef typename LeftOperand::Scalar Scalar;
    typedef CompoundIndex_t<FreeIndexTypeList> CompoundIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, CompoundIndex const &c)
    {
        // get the map which produces the CompoundIndex for each operand from the free indices c
        typedef CompoundIndexMap_t<FreeIndexTypeList,typename LeftOperand::FreeIndexTypeList> LeftOperandIndexMap;
        typedef CompoundIndexMap_t<FreeIndexTypeList,typename RightOperand::FreeIndexTypeList> RightOperandIndexMap;
        typename LeftOperandIndexMap::EvalMapType left_operand_index_map = LeftOperandIndexMap::eval;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;
        return left_operand[left_operand_index_map(c)] * right_operand[right_operand_index_map(c)];
    }
};

template <typename LeftOperand, typename RightOperand>
struct FreeIndexTypeListOfMultiplication_t
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // list of free indices from the left and right operands
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeIndexTypeList,
                                                typename RightOperand::FreeIndexTypeList>::T CombinedFreeIndexTypeList;
public:
    typedef typename ElementsHavingMultiplicity_t<CombinedFreeIndexTypeList,1>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct SummedIndexTypeListOfMultiplication_t
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // list of free indices from the left and right operands
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeIndexTypeList,
                                                typename RightOperand::FreeIndexTypeList>::T CombinedFreeIndexTypeList;
public:
    // the summed indices (at this level) are the double-occurrences indices
    // of the concatenated list of free indices from the left and right operands
    typedef typename ElementsHavingMultiplicity_t<CombinedFreeIndexTypeList,2>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct UsedIndexTypeListOfMultiplication_t
{
private:
    typedef typename SummedIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T SummedIndexTypeList;
public:
    // typelist of used indices which are prohibited from using higher up in the AST
    typedef typename UniqueTypesIn_t<
        typename ConcatenationOfTypeLists_t<
            typename ConcatenationOfTypeLists_t<typename LeftOperand::UsedIndexTypeList,
                                                typename RightOperand::UsedIndexTypeList>::T,
            SummedIndexTypeList>::T>::T T;
};

// TODO: there is an issue to think about: while it is totally valid to do
// u(i)*v(j)*w(j) (this is an outer product contracted with a vector), the
// expression v(j)*w(j) can be computed first and factored out of the whole
// thing, instead of needing to be multiplied out for each access of the i index.
// this may be somewhat difficult to do, as it would require searching the
// expression template AST for such contractions and restructuring the AST.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand>
struct ExpressionTemplate_Multiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                typename LeftOperand::Scalar,
                                typename FreeIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                typename UsedIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                 typename LeftOperand::Scalar,
                                 typename FreeIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                 typename UsedIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::CompoundIndex CompoundIndex;
    using Parent::IS_EXPRESSION_TEMPLATE;
    
    typedef typename SummedIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T SummedIndexTypeList;

    // TODO: check that the summed indices from each operand have no indices in common
    // though technically this is unnecessary, because the summed indices are "private"
    // to each contraction, so this is really for the human's benefit, not getting
    // confused by multiple repeated indices that have nothing to do with each other.
    enum
    {
        LEFT_OPERAND_IS_EXPRESSION_TEMPLATE  = Lvd::Meta::Assert<LeftOperand::IS_EXPRESSION_TEMPLATE>::v,
        RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE = Lvd::Meta::Assert<RightOperand::IS_EXPRESSION_TEMPLATE>::v,
        OPERAND_SCALAR_TYPES_ARE_EQUAL       = Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v>::v,
        FREE_INDICES_DONT_COLLIDE_WITH_USED  = Lvd::Meta::Assert<(!HasNontrivialIntersectionAsSets_t<FreeIndexTypeList,UsedIndexTypeList>::V)>::v
    };
    // TODO: ensure there are no indices that occur 3+ times (?)

    ExpressionTemplate_Multiplication_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    // available ONLY if FreeIndexTypeList is EmptyTypeList
    operator Scalar () const
    {
        Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<FreeIndexTypeList,EmptyTypeList>::v>();
        return operator[](CompoundIndex());
    }

    Scalar operator [] (CompoundIndex const &c) const
    {
        return BinarySummation_t<LeftOperand,RightOperand,FreeIndexTypeList,SummedIndexTypeList>::eval(m_left_operand, m_right_operand, c);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_left_operand.uses_tensor(t) || m_right_operand.uses_tensor(t);
    }

private:

    LeftOperand const &m_left_operand;
    RightOperand const &m_right_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// operator overloads for expression templates
// ////////////////////////////////////////////////////////////////////////////

// expression template addition/subtractions

// addition
template <typename LeftDerived, typename LeftFreeIndexTypeList, typename LeftUsedIndexTypeList,
          typename RightDerived, typename RightFreeIndexTypeList, typename RightUsedIndexTypeList>
ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>
    operator + (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFreeIndexTypeList,LeftUsedIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFreeIndexTypeList,RightUsedIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>(left_operand.as_derived(), right_operand.as_derived());
}

// subtraction
template <typename LeftDerived, typename LeftFreeIndexTypeList, typename LeftUsedIndexTypeList,
          typename RightDerived, typename RightFreeIndexTypeList, typename RightUsedIndexTypeList>
ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>
    operator - (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFreeIndexTypeList,LeftUsedIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFreeIndexTypeList,RightUsedIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>(left_operand.as_derived(), right_operand.as_derived());
}

// scalar multiplication/division, including unary negation (multiplication by -1)

// scalar multiplication on the right
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'> 
    operator * (ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand, 
                typename Derived::Scalar scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the right -- this overload allows integer literals to be used in scalar multiplications
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'> 
    operator * (ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand, 
                int scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the left
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'> 
    operator * (typename Derived::Scalar scalar_operand,
                ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the left -- this overload allows integer literals to be used in scalar multiplications
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'> 
    operator * (int scalar_operand,
                ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar division on the right
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'> 
    operator / (ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand, 
                typename Derived::Scalar scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>(operand.as_derived(), scalar_operand);
}

// scalar division on the right -- this overload allows integer literals to be used in scalar divisions (it's Scalar division, not integer division)
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'> 
    operator / (ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand, 
                int scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>(operand.as_derived(), scalar_operand);
}

// unary negation
template <typename Derived, typename FreeIndexTypeList, typename UsedIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'> 
    operator - (ExpressionTemplate_i<Derived,typename Derived::Scalar,FreeIndexTypeList,UsedIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), -1);
}

// expression template multiplication -- tensor contraction and product

template <typename LeftDerived, typename LeftFreeIndexTypeList, typename LeftUsedIndexTypeList,
          typename RightDerived, typename RightFreeIndexTypeList, typename RightUsedIndexTypeList>
ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>
    operator * (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFreeIndexTypeList,LeftUsedIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFreeIndexTypeList,RightUsedIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>(left_operand.as_derived(), right_operand.as_derived());
}

#endif // EXPRESSION_TEMPLATES_HPP_
