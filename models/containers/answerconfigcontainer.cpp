#include "containers/answerconfigcontainer.h"
#include "logics/tagrepository.h"
#include <functional>


QList<Tag>
AnswerConfigContainer::labelTags() const
{
    QList<Tag> labels;

    const std::function<bool(const QString&)> filter = [=](const QString& displayName){
        bool allowed = true;
        if (! excludeWord.isEmpty()) {
            allowed = (! displayName.contains(excludeWord));
        }
        if (allowed && (! includeWord.isEmpty())) {
            allowed = displayName.contains(includeWord);
        }
        return allowed;
    };

    if (! answerGroup.isEmpty()) {
        const TagGroup group = TagRepository().findGroup(answerGroup);
        if (group.exists()) {
            for (const Tag& t : group.tags()) {
                if (filter(t.displayName())) {
                    labels << t;
                }
            }
        }
    }

    return labels;
}
