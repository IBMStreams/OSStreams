/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPL_RUNTIME_TYPE_OPTIONAL_H
#define SPL_RUNTIME_TYPE_OPTIONAL_H

#include <stdlib.h>

#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Type/BaseOptional.h>
#include <SPL/Runtime/Type/ValueHandle.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

namespace SPL {

	class NativeByteBuffer;
	class NetworkByteBuffer;
	class VirtualByteBuffer;

	template<class T>
	class optional: public Optional {
		public:
			typedef T value_type;

			explicit optional() :
					_isPresent(false), _val() {
			}

			explicit optional(const value_type & val) :
					_isPresent(true), _val(val) {
			}

			optional(const optional<T> & ot) :
					_isPresent(ot._isPresent), _val(ot._val) {
			}

			optional(const ConstValueHandle & o) {
				Optional const & x = o;
				assignFrom(x);
			}

			virtual ~optional() {
			}

			virtual bool isPresent() const {
				return _isPresent;
			}

			virtual void setPresentWithDefaultValue() {
				_isPresent = true;
				// default value of underlying T is already constructed
			}

			T & value() {
				if (isPresent()) {
					return _val;
				}
				SPLTRACEMSGANDTHROW(SPLRuntimeNullValueAccess, L_ERROR, SPL_APPLICATION_RUNTIME_NULL_ACCESS,
						SPL_TYPE_DBG);
			}

			const T & value() const {
				if (isPresent()) {
					return _val;
				}
				SPLTRACEMSGANDTHROW(SPLRuntimeNullValueAccess, L_ERROR, SPL_APPLICATION_RUNTIME_NULL_ACCESS,
						SPL_TYPE_DBG);
			}

			size_t getSerializedSize() const {
				size_t sz = isPresent() ? SPL::getSerializedSize(_val) + 1 : 1;
				return sz;
			}

			optional<T> & operator=(const T & val) {
				_isPresent = true;
				_val = val;
				return *this;
			}

			virtual ValueHandle createValue() const {
				return ValueHandle(*(new T()));
			}

			virtual Optional * clone() const {
				return new optional<T>(*this);
			}

			virtual ValueHandle getValue() {
				if (isPresent()) {
					return ValueHandle(_val);
				}
				SPLTRACEMSGANDTHROW(SPLRuntimeNullValueAccess, L_ERROR, SPL_APPLICATION_RUNTIME_NULL_ACCESS,
						SPL_TYPE_DBG);
			}

			virtual ConstValueHandle getValue() const {
				if (isPresent()) {
					return ConstValueHandle(_val);
				}
				SPLTRACEMSGANDTHROW(SPLRuntimeNullValueAccess, L_ERROR, SPL_APPLICATION_RUNTIME_NULL_ACCESS,
						SPL_TYPE_DBG);
			}

			virtual Meta::Type getValueMetaType() const {
				return Meta::Type::typeOf<T>();
			}

			virtual void swapWith(Optional & ot) {
				if (!SPL::equals(typeid(*this), typeid(ot))) {
					SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR,
							SPL_APPLICATION_RUNTIME_INVALID_OPTIONAL_TYPE(typeid(T).name()), SPL_TYPE_DBG);
				}
				optional<T> & o = static_cast<optional<T> &>(ot);
				optional<T> t(o);
				o = *this;
				*this = t;
			}

			virtual void assignFrom(const Optional & ot) {
				if (!SPL::equals(typeid(*this), typeid(ot))) {
					SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR,
							SPL_APPLICATION_RUNTIME_INVALID_OPTIONAL_TYPE(typeid(T).name()), SPL_TYPE_DBG);
				}
				optional<T> const & o = static_cast<optional<T> const &>(ot);
				_isPresent = o._isPresent;
				_val = o._val;
			}

			virtual bool equals(const Optional & ot) const {
				if (!SPL::equals(typeid(*this), typeid(ot))) {
					SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR,
							SPL_APPLICATION_RUNTIME_INVALID_OPTIONAL_TYPE(typeid(T).name()), SPL_TYPE_DBG);
				}
				optional<T> const & o = static_cast<optional<T> const &>(ot);
				return *this == o;
			}

			virtual void setValue(const ConstValueHandle & v) {
				if (v.getMetaType() == Meta::Type::OPTIONAL) {
					const Optional & ot = (const Optional &) v;
					assignFrom(ot);
				} else {
					const T & ot = v;
					operator=(ot);
				}
			}

			bool operator==(const SPL::optional<T> & other) const {
                            if (!_isPresent && !other._isPresent) {
                                return true;
                            }
                            if (_isPresent && other._isPresent) {
                                return _val == other._val;
                            }
                                return false;
			}

			bool operator!=(const SPL::optional<T> & other) const {
				return !(*this == other);
			}

			/// Reset to null
			void clear();

			/// Serialize (character)
			/// @param ostr output stream
			virtual void serialize(std::ostream & ostr) const;

			/// Deserialize (character)
			/// @param istr input stream
			/// @param withSuffix if true then consume suffix, otherwise (false)
			/// assume no suffix is present
			virtual void deserialize(std::istream & istr, bool withSuffix = false);

#ifndef DOXYGEN_SKIP_FOR_USERS
			virtual void serialize(VirtualByteBuffer & sbuf) const {
				if (isPresent()) {
					SPL::serializeBoolean(sbuf, true);
					sbuf << _val;
				}
				else {
					SPL::serializeBoolean(sbuf, false);
				}
			}

			virtual void deserialize(VirtualByteBuffer & sbuf) {
				this->clear();
				bool isNull = SPL::deserializeBoolean(sbuf);
				if (!isNull) {
					sbuf >> _val;
					_isPresent = true;
				}
			}
			virtual const std::type_info & getTypeId() const {
				return typeid(T);
			}

#endif /* DOXYGEN_SKIP_FOR_USERS */

			/// Serialize (binary)
			/// @param buf byte buffer to use
			template<class BufferType>
			void serialize(ByteBuffer<BufferType> & sbuf) const {
				if (isPresent()) {
					SPL::serializeBoolean(sbuf, true);
					sbuf << _val;
				}
				else {
					SPL::serializeBoolean(sbuf, false);
				}
			}

			/// Deserialize (binary)
			/// @param buf byte buffer to use
			template<class BufferType>
			void deserialize(ByteBuffer<BufferType> & sbuf) {
				this->clear();
				bool isNull = SPL::deserializeBoolean(sbuf);
				if (!isNull) {
					sbuf >> _val;
					_isPresent = true;
				}
			}

			/// Deserialize with Inf and Nan (character)
			/// @param istr input stream
			/// @param withSuffix if true then consume suffix, otherwise (false)
			/// assume no suffix is present
			void deserializeWithNanAndInfs(std::istream & istr,bool withSuffix=false);

			/// Get hash code
			/// @return hash code
			virtual size_t hashCode() const {
				return (!isPresent()) ? 37 : std::tr1::hash<T>()(_val);
			}

		private:
			bool _isPresent;
			T _val;
	};

#define DECL_CLEAR_SPECIALIATION(type)   \
template <> inline void optional<type>::clear() {_isPresent = false; _val = (type)0; }

	DECL_CLEAR_SPECIALIATION(float32)
	DECL_CLEAR_SPECIALIATION(float64)
	DECL_CLEAR_SPECIALIATION(decimal32)
	DECL_CLEAR_SPECIALIATION(decimal64)
	DECL_CLEAR_SPECIALIATION(decimal128)
	DECL_CLEAR_SPECIALIATION(complex32)
	DECL_CLEAR_SPECIALIATION(complex64)
	DECL_CLEAR_SPECIALIATION(boolean)
	DECL_CLEAR_SPECIALIATION(uint8)
	DECL_CLEAR_SPECIALIATION(uint16)
	DECL_CLEAR_SPECIALIATION(uint32)
	DECL_CLEAR_SPECIALIATION(uint64)
	DECL_CLEAR_SPECIALIATION(int8)
	DECL_CLEAR_SPECIALIATION(int16)
	DECL_CLEAR_SPECIALIATION(int32)
	DECL_CLEAR_SPECIALIATION(int64)
	DECL_CLEAR_SPECIALIATION(timestamp)
#undef CLEAR_SPECIALIATION

	template<>
	inline void optional<ustring>::clear() {
		_isPresent = false;
		_val.remove();
	}

	template<class T>
	inline void optional<T>::clear() {
		_isPresent = false;
		_val.clear();
	}

	template<class T>
	inline std::ostream & operator <<(std::ostream & ostr, const optional<T> & val) {
		val.serialize(ostr);
		return ostr;
	}

	template<class T>
	inline std::istream & operator >>(std::istream & istr, optional<T> & val) {
		val.deserialize(istr);
		return istr;
	}

	template<class T>
	void optional<T>::serialize(std::ostream & ostr) const {
		if (isPresent()) {
			ostr << _val;
		}
		else {
			ostr << "null";
		}
	}

	template<class T>
	void optional<T>::deserialize(std::istream & istr, bool withSuffix) {
		this->clear();
		int c;

		// Skip leading whitespace
		for (;;) {
			c = istr.peek();
                        if (!istr) {
                            return;
                        }
                        if (c == '\t' || c == ' ') {
				c = istr.get();
			}
			else {
				break;
			}
		}

		// istr could be null, in which case we are done
		c = istr.peek();
                if (!istr) {
                    return;
                }

                if (c == 'n') {
			char v[4];
                        // std::streamsize read = istr.readsome(v, 4);
			istr.read(v, 4);
                        if (!istr) {
                            return;
                        }
                        std::streamsize read = istr.gcount();
			if (read == 4) {
				if (v[1] == 'u' && v[2] == 'l' && v[3] == 'l') {
					return;
				}
			}
			for (int i = read - 1; i >= 0; --i) {
				istr.putback(v[i]);
			}
		}
		if (withSuffix) {
			SPL::deserializeWithSuffix(istr, _val);
		}
		else {
			istr >> _val;
		}
		_isPresent = true;
	}

	template<class T>
	void optional<T>::deserializeWithNanAndInfs(std::istream & istr, bool withSuffix) {
		this->clear();
		int c;

		// Skip leading whitespace
		for (;;) {
			c = istr.peek();
                        if (!istr) {
                            return;
                        }
                        if (c == '\t' || c == ' ') {
				c = istr.get();
			}
			else {
				break;
			}
		}

		// istr could be null, in which case we are done
		c = istr.peek();
                if (!istr) {
                    return;
                }

                if (c == 'n') {
			char v[4];
			istr.read(v, 4);
                        if (!istr) {
                            return;
                        }
                        std::streamsize read = istr.gcount();
			if (read == 4) {
				if (v[1] == 'u' && v[2] == 'l' && v[3] == 'l') {
					return;
				}
			}
			for (int i = read - 1; i >= 0; --i) {
				istr.putback(v[i]);
			}
		}
		SPL::deserializeWithNanAndInfs(istr,_val,withSuffix);
		_isPresent = true;
	}

#ifndef DOXYGEN_SKIP_FOR_USERS
	template<class T>
	inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, optional<T> const & val) {
		val.serialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, optional<T> & val) {
		val.deserialize(sbuf);
		return sbuf;
	}
#endif /* DOXYGEN_SKIP_FOR_USERS */

	template<class S, class T>
	inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, optional<T> & val) {
		val.deserialize(sbuf);
		return sbuf;
	}

	template<class S, class T>
	inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const optional<T> & val) {
		val.serialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, optional<T> const & val) {
		val.serialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, optional<T> & val) {
		val.deserialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, optional<T> const & val) {
		val.serialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, optional<T> & val) {
		val.deserialize(sbuf);
		return sbuf;
	}

	template<class T>
	inline void deserializeWithNanAndInfs(std::istream & istr, optional<T> & val, bool withSuffix = false) {
                val.deserializeWithNanAndInfs(istr, withSuffix);
	}

} // namespace SPL

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
	namespace tr1 {
		template<class T>
		struct hash<SPL::optional<T> > {
				inline size_t operator ()(const SPL::optional<T> & l) const {
					return l.hashCode();
				}
		};
	}
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

#endif
