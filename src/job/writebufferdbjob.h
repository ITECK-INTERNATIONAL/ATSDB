/*
 * This file is part of ATSDB.
 *
 * ATSDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ATSDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ATSDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WRITEBUFFERDBJOB_H_
#define WRITEBUFFERDBJOB_H_

#include <list>

#include "DBJob.h"

class Buffer;

/**
 * @brief Buffer write job
 *
 * Writes buffer's data contents to a database table
 */
class WriteBufferDBJob : public DBJob
{
  public:
    WriteBufferDBJob(JobOrderer* orderer, boost::function<void(Job*)> done_function,
                     boost::function<void(Job*)> obsolete_function, DBInterface* db_interface,
                     Buffer* buffer);

    virtual ~WriteBufferDBJob();

    virtual void execute();

    Buffer* getBuffer() { return buffer_; }

  protected:
    Buffer* buffer_;
};

#endif /* WRITEBUFFERDBJOB_H_ */
