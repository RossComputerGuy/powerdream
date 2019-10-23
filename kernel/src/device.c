#include <kernel/device.h>
#include <kernel/error.h>

PD_LIST_HEAD(devices, pd_dev_t);

static struct devices devices;

pd_dev_t* pd_dev_fromdev(dev_t dev) {
  pd_dev_t* device = NULL;
  PD_SLIST_FOREACH(device, &devices, d_list) {
    if (device->dev == dev) return device;
  }
  return NULL;
}

int pd_dev_register(pd_dev_t* device) {
  if (pd_dev_fromdev(device->dev) != NULL) return -EEXIST;
  PD_SLIST_INSERT_HEAD(&devices, device, d_list);
  return 0;
}