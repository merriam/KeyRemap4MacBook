#ifndef FLAGSTATUS_HPP
#define FLAGSTATUS_HPP

#include "KeyCode.hpp"
#include "bridge.h"

namespace org_pqrs_KeyRemap4MacBook {
  class FlagStatus {
  public:
    class Item {
      friend class FlagStatus;

    public:
      Item(void) :
        flag_(ModifierFlag::ZERO),
        count_(0),
        temporary_count_(0),
        lock_count_(0),
        negative_lock_count_(0),
        sticky_count_(0),
        lazy_count_(0),
        lazy_enabled_(false)
      {}

    private:
      void initialize(ModifierFlag f);
      void set(void);
      void set(KeyCode key, Flags flags);

      void reset(void);
      int sum(bool forceEnableLazy) const {
        int sum = count_ + temporary_count_ + lock_count_ - negative_lock_count_ + sticky_count_;
        if (forceEnableLazy || lazy_enabled_) {
          sum += lazy_count_;
        }
        return sum;
      }

      void increase(void);
      void decrease(void);
      void temporary_increase(void) { ++temporary_count_; }
      void temporary_decrease(void) { --temporary_count_; }
      void lock_increase(void) { lock_count_ = 1; }
      void lock_decrease(void) { lock_count_ = 0; }
      void lock_toggle(void)   { lock_count_ = ! lock_count_; }
      void negative_lock_increase(void) { negative_lock_count_ = 1; }
      void negative_lock_decrease(void) { negative_lock_count_ = 0; }
      void negative_lock_toggle(void)   { negative_lock_count_ = ! negative_lock_count_; }
      void sticky_increase(void) { sticky_count_ = 1; }
      void sticky_decrease(void) { sticky_count_ = 0; }
      void sticky_toggle(void) { sticky_count_ = ! sticky_count_; }
      void lazy_increase(void) { ++lazy_count_; }
      void lazy_decrease(void) { --lazy_count_; }
      void lazy_enable(void) { lazy_enabled_ = true; }
      /*
       * The following key sequence is required with lazy command key + tab:
       * (== We should not disable lazy command key when the tab key is released.)
       *
       * eventType:keyMod          code:0x37       name:Command_L       flags:Cmd
       * eventType:keyDown         code:0x30       name:Tab             flags:Cmd
       * eventType:keyUp           code:0x30       name:Tab             flags:Cmd
       * eventType:keyDown         code:0x30       name:Tab             flags:Cmd
       * eventType:keyUp           code:0x30       name:Tab             flags:Cmd
       * eventType:keyMod          code:0x37       name:Command_L       flags:
       *
       *
       * The following key sequence is required with lazy command key + "command+space to left click".
       * (== We have to disable lazy command key when the space key is released.)
       *
       * eventType:mouseDown       code:0x0        name:left            flags:
       * eventType:mouseUp         code:0x0        name:left            flags:
       * eventType:mouseDown       code:0x0        name:left            flags:
       * eventType:mouseUp         code:0x0        name:left            flags:
       *
       *
       * Therefore, we have to disable lazy command key only when it is not active.
       */
      void lazy_disable_if_off(void) {
        if (sum(true) <= 0) {
          lazy_enabled_ = false;
        }
      }

      ModifierFlag flag_;
      int count_;
      int temporary_count_;

      int lock_count_; // store remapped lock status. (CapsLock, FN lock, ...)
      int negative_lock_count_;

      int sticky_count_;

      int lazy_count_;
      bool lazy_enabled_;
    };
    DECLARE_VECTOR(Item);

    FlagStatus(void);
    FlagStatus(Flags flags);

    void initialize(void);
    void set(void);
    void set(KeyCode key, Flags flags);
    Flags makeFlags(void) const;
    // get registered ModifierFlag by index.
    ModifierFlag getFlag(size_t index) const;
    void reset(void);

    size_t itemSize(void) const { return item_.size(); }

    bool isOn(const Vector_ModifierFlag& modifierFlags) const;

#define DECLARE_METHODS(METHOD)                                                     \
  void METHOD(ModifierFlag modifierFlag);                                           \
  void METHOD(ModifierFlag modifierFlag, const Vector_ModifierFlag &modifierFlags); \
  void METHOD(const Vector_ModifierFlag &modifierFlags);                            \

    DECLARE_METHODS(increase)
    DECLARE_METHODS(decrease)
    DECLARE_METHODS(temporary_increase)
    DECLARE_METHODS(temporary_decrease)
    DECLARE_METHODS(lock_increase)
    DECLARE_METHODS(lock_decrease)
    DECLARE_METHODS(lock_toggle)
    DECLARE_METHODS(negative_lock_increase)
    DECLARE_METHODS(negative_lock_decrease)
    DECLARE_METHODS(negative_lock_toggle)
    DECLARE_METHODS(sticky_increase)
    DECLARE_METHODS(sticky_decrease)
    DECLARE_METHODS(sticky_toggle)
    DECLARE_METHODS(lazy_increase)
    DECLARE_METHODS(lazy_decrease)
#undef DECLARE_METHODS

    void sticky_active_modifiers_toggle(void);
    void sticky_active_modifiers_increase(void);
    void sticky_active_modifiers_decrease(void);

    void sticky_clear(void);
    void lock_clear(void);
    void negative_lock_clear(void);
    void lazy_enable(void);
    void lazy_disable_if_off(void);

    void subtract(const FlagStatus& other, Vector_ModifierFlag& modifierFlags) const;

    static FlagStatus& globalFlagStatus(void);

  private:
    bool isOn(ModifierFlag modifierFlag) const;
    void updateStatusMessage(void);
    void updateStatusMessage(unsigned int statusMessageIndex);

    Vector_Item item_;
  };
}

#endif
