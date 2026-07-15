/******************************************************************************
 * File: shared_result_client.h
 * Author: Ander
 *
 * Description:
 * Encapsulates the POSIX shared memory contract defined in
 * compartido.h (owned by Process 1). This process only opens and
 * writes to the segment -- it never creates or destroys it.
 ******************************************************************************/
#ifndef SHARED_RESULT_CLIENT_H
#define SHARED_RESULT_CLIENT_H

int shared_result_client_init(void);
void shared_result_client_write(int digit);
void shared_result_client_close(void);

#endif /* SHARED_RESULT_CLIENT_H */