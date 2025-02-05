/////////////////////////////////////////////////////////////////////////////
// Name:        vector.h
// Purpose:     interface of wxVector<T>
// Author:      wxWidgets team
// RCS-ID:      $Id: vector.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

    wxVector<T> is a template class which implements most of the @c std::vector
    class and can be used like it.

    If wxWidgets is compiled in STL mode, wxVector will just be a typedef to
    @c std::vector. Just like for @c std::vector, objects stored in wxVector<T>
    need to be @e assignable but don't have to be @e "default constructible".

    Please refer to the STL documentation for further information.

    @nolibrary
    @category{containers}

    @see @ref overview_container, wxList<T>, wxArray<T>, wxVectorSort<T>
*/
template<typename T>
class wxVector<T>
{
public:
    typedef size_t size_type;
    typedef size_t difference_type;
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;

    /**
        Reverse iterator interface
    */
    class reverse_iterator
    {
    public:
        reverse_iterator();
        explicit reverse_iterator(iterator it);
        reverse_iterator(const reverse_iterator& it);
        reference operator*() const;
        pointer operator->() const;
        iterator base() const;
        reverse_iterator& operator++();
        reverse_iterator operator++(int);
        reverse_iterator& operator--();
        reverse_iterator operator--(int);
        reverse_iterator operator+(difference_type n) const;
        reverse_iterator& operator+=(difference_type n);
        reverse_iterator operator-(difference_type n) const;
        reverse_iterator& operator-=(difference_type n);
        reference operator[](difference_type n) const;
        bool operator ==(const reverse_iterator& it) const;
        bool operator !=(const reverse_iterator& it) const;
    };

    /**
        Constructor.
    */
    wxVector();

    /**
        Constructor initializing the vector with the given number of
        default-constructed objects.
     */
    wxVector(size_type size);

    /**
        Constructor initializing the vector with the given number of
        copies of the given object.
     */
    wxVector(size_type size, const value_type& value);

    /**
        Copy onstructor.
    */
    wxVector(const wxVector<T>& c);

    /**
        Destructor.
    */
    ~wxVector();

    /**
        Returns item at position @a idx.
    */
    const value_type& at(size_type idx) const;

    /**
        Returns item at position @a idx.
    */
    value_type& at(size_type idx);

    /**
        Return the last item.
    */
    const value_type& back() const;

    /**
        Return the last item.
    */
    value_type& back();

    /**
        Return iterator to beginning of the vector.
    */
    const_iterator begin() const;

    /**
        Return iterator to beginning of the vector.
    */
    iterator begin();

    /**
        Return reverse iterator to end of the vector.
    */
    reverse_iterator rbegin();

    /**
        Return reverse iterator to beginning of the vector.
    */
    reverse_iterator rend();


    /**
        Returns vector's current capacity, i.e. how much memory is allocated.

        @see reserve()
    */
    size_type capacity() const;

    /**
        Clears the vector.
    */
    void clear();

    /**
        Returns @true if the vector is empty.
    */
    bool empty() const;

    /**
        Returns iterator to the end of the vector.
    */
    const_iterator end() const;

    /**
        Returns iterator to the end of the vector.
    */
    iterator end();

    /**
        Erase item pointed to by iterator @a it.

        @return Iterator pointing to the item immediately after the erased one.
    */
    iterator erase(iterator it);

    /**
        Erase items in the range @a first to @a last (@a last is not erased).

        @return Iterator pointing to the item immediately after the erased
                range.
    */
    iterator erase(iterator first, iterator last);

    /**
        Returns the first item.
    */
    const value_type& front() const;

    /**
        Returns the first item.
    */
    value_type& front();

    /**
        Insert item @a v at given position @a it.

        @return Iterator for the inserted item.
    */
    iterator insert(iterator it, const value_type& v = value_type());

    /**
        Assignment operator.
    */
    wxVector& operator=(const wxVector& vb);

    /**
        Returns item at position @a idx.
    */
    const value_type& operator[](size_type idx) const;

    /**
        Returns item at position @a idx.
    */
    value_type& operator[](size_type idx);

    /**
        Removes the last item.
    */
    void pop_back();

    /**
        Adds an item to the end of the vector.
    */
    void push_back(const value_type& v);

    /**
        Reserves memory for at least @a n items.

        @see capacity()
    */
    void reserve(size_type n);

    /**
        Makes the vector of size @a n.

        If @a n is less than the current size(), the elements at the end of the
        vector are erased. If it is greater, then the vector is completed with
        either the copies of the given object @a v or @c value_type() objects
        until it becomes of size @a n.
     */
    //@{
    void resize(size_type n);
    void resize(size_type n, const value_type& v);
    //@}

    /**
        Returns the size of the vector.
    */
    size_type size() const;
};


/**
   Sort the contents of a @c wxVector<T>.  In a STL build this function will
   be defined as a thin wrapper around std::sort.  To be sortable the
   contained type must support the less-than operator.

   @code
   wxVector<SomeClass> v;
   ... // items are added to the vector v...
   wxVectorSort(v);
   @endcode
      
   @see wxVector<T>
*/
template<typename T>
void wxVectorSort(wxVector<T>& v);
