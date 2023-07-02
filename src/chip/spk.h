#ifndef _SPK_H_
#define _SPK_H_

#include <utility>
#include <list>
#include <vector>

class spk
{
    friend class neuron;
    friend class synapse;
    friend class core;
    /*---------------------fields-----------------*/
    private: double _spkTime;
    private: std::vector<std::tuple<int32_t, int32_t, int32_t>> _spkPosition; // core_num, v_or_h, neuron_num
    /*---------------------methods----------------*/
    // spk Class constructor.
    public: spk(double _spkTime, std::vector<std::tuple<int32_t, int32_t, int32_t>> pos);
    public: spk();
    
    // assignment operator
    public: spk& operator = (const spk&);
    
    // relational operators

    public: bool operator == (const spk&) const;
    public: bool operator != (const spk&) const;
    public: bool operator <  (const spk&) const;
    public: bool operator <= (const spk&) const;
    public: bool operator >  (const spk&) const;
    public: bool operator >= (const spk&) const;

    // arithmetic operators
    public: spk& operator += (const spk&);
    public: spk& operator -= (const spk&);

    public: friend const spk operator + (const spk&, const spk&);
    public: friend const spk operator - (const spk&, const spk&);


};
//----------------------------------------------------
// constructors
inline spk::spk() : _spkTime(0),_spkPosition(NULL){}

// assignment operator

inline psml_time& psml_time::operator = (const psml_time& t)
{
    _m_value = t._m_value;
    return *this;
}

// relational operators

inline bool spk::operator == (const spk& t) const
{
	return (_spkTime == t._spkTime);
}

inline bool spk::operator != (const spk& t) const
{
	return (_spkTime != t._spkTime);
}

inline bool spk::operator < (const spk& t) const
{
	return (_spkTime < t._spkTime);
}

inline bool spk::operator <= (const spk& t) const
{
	return (_spkTime <= t._spkTime);
}

inline bool spk::operator > (const spk& t) const
{
	return (_spkTime > t._spkTime);
}

inline bool spk::operator >= (const spk& t) const
{
	return (_spkTime >= t._spkTime);
}

// arithmetic operators

inline spk& spk::operator += (const spk& t)
{
    if (_spkTime == t._spkTime) {
        for (auto i = _spkPosition.begin(), i = _spkPosition.end(), i++) {
            for (auto j = t._spkPosition.begin(), j = t._spkPosition.end(), j++) {
                if (i == j) {
                    i.push_back(j);
                }
            }
        }
    }
	return *this;
}

inline spk& spk::operator -= (const spk& t)
{
    if (_spkTime == t._spkTime) {
        for (auto i = _spkPosition.begin(), i = _spkPosition.end(), i++) {
            for (auto j = t._spkPosition.begin(), j = t._spkPosition.end(), j++) {
                if (i == j) {
                    i.pop_back(j);
                }
            }
        }
    }
    return *this;
}


inline const spk operator + (const spk& t1, const spk& t2)
{
	return t1 += t2;
}

inline const spk operator -(const spk& t1, const spk& t2)
{
	return t1 -= t2;
}


auto spk_comparator = [](spk a, spk b) {
    return a.spk_timing > b.spk_timing;
};

#endif /* _SPK_H_ */
