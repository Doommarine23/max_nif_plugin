/**********************************************************************
*<
FILE: AnimKey.h

DESCRIPTION:	Animation Key Import Routines

CREATED BY: tazpn (Theo)

HISTORY: 

*>	Copyright (c) 2006, All Rights Reserved.
**********************************************************************/
// Max Headers
#include <Max.h>
#include <tab.h>

// Niflib Headers
#include <niflib.h>
#include <Key.h>
using namespace Niflib;

const float FramesPerSecond = 30.0f;
const float FramesIncrement = 1.0f/30.0f;
const int TicksPerFrame = GetTicksPerFrame();

inline TimeValue TimeToFrame(float t) {
   return TimeValue(t * FramesPerSecond * TicksPerFrame);
}
inline float FrameToTime(TimeValue t) {
   return float(t) / (FramesPerSecond * TicksPerFrame);
}

typedef Key<float> FloatKey;
typedef Key<Quaternion> QuatKey;
typedef Key<Vector3> Vector3Key;


template<typename T, typename U> T MapKey(U& key, float time);

// Specialized Linear Mappings
template<> ILinFloatKey MapKey<ILinFloatKey,FloatKey>(FloatKey& key, float time);
template<> ILinRotKey MapKey<ILinRotKey, QuatKey>(QuatKey& key, float time);
template<> ILinScaleKey MapKey<ILinScaleKey, FloatKey>(FloatKey& key, float time);
template<> ILinPoint3Key MapKey<ILinPoint3Key, Vector3Key>(Vector3Key& key, float time);

template<> FloatKey MapKey<FloatKey, ILinFloatKey>(ILinFloatKey& key, float time);
template<> QuatKey MapKey<QuatKey, ILinRotKey>(ILinRotKey& key, float time);
template<> FloatKey MapKey<FloatKey, ILinScaleKey>(ILinScaleKey& key, float time);
template<> Vector3Key MapKey<Vector3Key, ILinPoint3Key>(ILinPoint3Key& key, float time);

// Specialized Bezier/Hybrid mappings
template<> IBezFloatKey MapKey<IBezFloatKey, FloatKey>(FloatKey& key, float time);
template<> IBezPoint3Key MapKey<IBezPoint3Key, Vector3Key>(Vector3Key& key, float time);
template<> IBezQuatKey MapKey<IBezQuatKey, QuatKey>(QuatKey& key, float time);
template<> IBezScaleKey MapKey<IBezScaleKey, FloatKey>(FloatKey& key, float time);

template<> FloatKey MapKey<FloatKey, IBezFloatKey>(IBezFloatKey& key, float time);
template<> QuatKey MapKey<QuatKey, IBezQuatKey>(IBezQuatKey& key, float time);
template<> FloatKey MapKey<FloatKey, IBezScaleKey>(IBezScaleKey& key, float time);
template<> Vector3Key MapKey<Vector3Key, IBezPoint3Key>(IBezPoint3Key& key, float time);

// Specialized TCB mappings
template<> ITCBFloatKey MapKey<ITCBFloatKey, FloatKey>(FloatKey& key, float time);
template<> ITCBRotKey MapKey<ITCBRotKey, QuatKey>(QuatKey& key, float time);
template<> ITCBPoint3Key MapKey<ITCBPoint3Key, Vector3Key>(Vector3Key& key, float time);
template<> ITCBScaleKey MapKey<ITCBScaleKey, FloatKey>(FloatKey& key, float time);

template<> FloatKey MapKey<FloatKey, ITCBFloatKey>(ITCBFloatKey& key, float time);
template<> QuatKey MapKey<QuatKey, ITCBRotKey>(ITCBRotKey& key, float time);
template<> FloatKey MapKey<FloatKey, ITCBScaleKey>(ITCBScaleKey& key, float time);
template<> Vector3Key MapKey<Vector3Key, ITCBPoint3Key>(ITCBPoint3Key& key, float time);


template<typename T> void MergeKey(T& lhs, T& rhs) {
   lhs.val += rhs.val;
}

//template<> void MergeKey<ILinFloatKey>(ILinFloatKey& lhs, ILinFloatKey& rhs);
template<> void MergeKey<ILinRotKey>(ILinRotKey& lhs, ILinRotKey& rhs);
//template<> void MergeKey<ILinScaleKey>(ILinScaleKey& lhs, ILinScaleKey& rhs);
//template<> void MergeKey<ILinPoint3Key>(ILinPoint3Key& lhs, ILinPoint3Key& rhs);

//template<> void MergeKey<IBezFloatKey>(IBezFloatKey& lhs, IBezFloatKey& rhs);
template<> void MergeKey<IBezQuatKey>(IBezQuatKey& lhs, IBezQuatKey& rhs);
//template<> void MergeKey<IBezPoint3Key>(IBezPoint3Key& lhs, IBezPoint3Key& rhs);
//template<> void MergeKey<IBezScaleKey>(IBezScaleKey& lhs, IBezScaleKey& rhs);

//template<> void MergeKey<ITCBFloatKey>(ITCBFloatKey& lhs, ITCBFloatKey& rhs);
template<> void MergeKey<ITCBRotKey>(ITCBRotKey& lhs, ITCBRotKey& rhs);
//template<> void MergeKey<ITCBPoint3Key>(ITCBPoint3Key& lhs, ITCBPoint3Key& rhs);
//template<> void MergeKey<ITCBScaleKey>(ITCBScaleKey& lhs, ITCBScaleKey& rhs);

template<typename T> void MergeKey(IKeyControl *keys, T& key)
{
   for (int i=0, n=keys->GetNumKeys(); i<n; ++i)
   {
      T tmp; keys->GetKey(i, &tmp);
      if (tmp.time == key.time) {
         MergeKey(tmp, key);
         keys->SetKey(i, &tmp);
         return;
      }
   }
   keys->AppendKey(&key);
}

template<typename T, typename U>
inline void SetKeys(Control *subCtrl, vector<U>& keys, float time)
{
   if (subCtrl && !keys.empty()){
      if (IKeyControl *ikeys = GetKeyControlInterface(subCtrl)){
         ikeys->SetNumKeys(keys.size());
         for (int i=0,n=keys.size(); i<n; ++i) {
            ikeys->SetKey(i, &MapKey<T>(keys[i], time));
         }
         ikeys->SortKeys();
      }
   }
}

template<typename T, typename U>
inline void GetKeys(Control *subCtrl, vector<T>& keys, float time)
{
   if (IKeyControl *ikeys = GetKeyControlInterface(subCtrl)){
      int n = ikeys->GetKeySize();
      keys.reserve(n);
      for (int i=0; i<n; ++i){
         U key;
         ikeys->GetKey(i, &key);
         keys.push_back( MapKey<T>(key, time) );
      }
   }
}

template<typename T, typename U>
inline void MergeKeys(Control *subCtrl, vector<U>& keys, float time)
{
   if (subCtrl && !keys.empty()){
      if (IKeyControl *ikeys = GetKeyControlInterface(subCtrl)){
         if (ikeys->GetNumKeys() == 0){
            SetKeys<T,U>(subCtrl, keys, time);
         } else {
            for (int i=0,n=keys.size(); i<n; ++i) {
               MergeKey(ikeys, MapKey<T>(keys[i], time));
            }
            ikeys->SortKeys();
         }        
      }
   }
}

enum V3T
{
   V3T_X, V3T_Y, V3T_Z
};
extern float GetValue(Vector3& value, V3T type);
extern FloatKey& CopyKey( FloatKey& dst, Vector3Key& src, V3T type);
extern void SplitKeys(vector<Vector3Key>&keys, vector<FloatKey>&xkeys, vector<FloatKey>&ykeys, vector<FloatKey>&zkeys);
extern void JoinKeys(vector<Vector3Key>&keys, vector<FloatKey>&xkeys, vector<FloatKey>&ykeys, vector<FloatKey>&zkeys);

typedef Key<string> KeyTextValue;

bool GetTranslationKeys(Control *c, vector<Vector3Key> keys, const vector<float>& times, float timeOffset=0.0f);