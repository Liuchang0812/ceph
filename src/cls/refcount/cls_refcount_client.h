#ifndef CEPH_CLS_REFCOUNT_CLIENT_H
#define CEPH_CLS_REFCOUNT_CLIENT_H

#include "include/types.h"

namespace librados {
  class ObjectWriteOperation;
  class IoCtx;
}

/*
 * refcount objclass
 *
 * The refcount objclass implements a refcounting scheme that allows having multiple references
 * to a single rados object. The canonical way to use it is to add a reference and to remove a
 * reference using a specific tag. This way we ensure that refcounting operations are idempotent,
 * that is, a single client can only increase/decrease the refcount once using a single tag, so
 * any replay of operations (implicit or explicit) is possible.
 *
 * So, the regular usage would be to create an object, to increase the refcount. Then, when
 * wanting to have another reference to it, increase the refcount using a different tag. When
 * removing a reference it is required to drop the refcount (using the same tag that was used
 * for that reference). When the refcount drops to zero, the object is removed automaticfally.
 *
 * In order to maintain backwards compatibility with objects that were created without having
 * their refcount increased, the implicit_ref was added. Any object that was created without
 * having it's refcount increased (explicitly) is having an implicit refcount of 1. Since
 * we don't have a tag for this refcount, we consider this tag as a wildcard. So if the refcount
 * is being decreased by an unknown tag and we still have one wildcard tag, we'll accept it
 * as the relevant tag, and the refcount will be decreased.
 */

void cls_refcount_get(librados::ObjectWriteOperation& op, const std::string& tag, bool implicit_ref = false);
void cls_refcount_put(librados::ObjectWriteOperation& op, const std::string& tag, bool implicit_ref = false);
void cls_refcount_set(librados::ObjectWriteOperation& op, std::list<std::string>& refs);
int cls_refcount_read(librados::IoCtx& io_ctx, std::string& oid, std::list<std::string> *refs, bool implicit_ref = false);

#endif
