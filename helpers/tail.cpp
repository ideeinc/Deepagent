#include <QFileInfo>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "tail.h"


Tail::Tail(const QString &filePath)
    : ApplicationHelper(), path(filePath)
{ }


QByteArray Tail::tail(int maxlen, qint64 offset) const
{
    const qint64 BUFSIZE = sysconf(_SC_PAGE_SIZE);

    QByteArray ret;
    ret.reserve(qMin(maxlen, (int)BUFSIZE));

    if (Q_UNLIKELY(path.isEmpty())) {
        return ret;
    }

    int fd = open(path.toLocal8Bit(), O_RDONLY);
    if (Q_UNLIKELY(fd < 0)) {
        // error
        return ret;
    }

    qint64 fsize = QFileInfo(path).size();

    if (maxlen <= 0) {
        maxlen = INT_MAX;
    }

    while (offset < fsize && ret.length() < maxlen) {
        qint64 paOffset = offset & ~(BUFSIZE - 1);  // must be page aligned
        const qint64 len = qMin(qMin(BUFSIZE, fsize - paOffset), (qint64)maxlen - ret.length());

        if (len > offset - paOffset) {
            char *addr = (char*)mmap(nullptr, len, PROT_READ, MAP_PRIVATE, fd, paOffset);
            if (addr == MAP_FAILED) {
                // error
                break;
            }

            ret.append(addr + offset - paOffset, len - (offset - paOffset));  // deep copy
            munmap(addr, len);
            offset = paOffset + len;
        }

        // next loop
        fsize = QFileInfo(path).size();
    }
    close(fd);
    return ret;
}
