#!/bin/sh

printUsage() {
    echo ""
    echo "Usage: $0 <command> <filter name> <id> <namespace>"
    echo -e "command: deploy or undeploy"
    echo -e "filter name: the name and tag of filter to deploy/undeploy"
    echo -e "id: the id for this filter"
    echo -e "namespace: to apply this filter"
    exit 1
}

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
    printUsage
fi

if [ "$1" != "deploy" ] && [ "$1" != "undeploy" ]; then
    printUsage
fi

command=$1
filter_name=$2
id=$3
namespace=$4

app_labels=$(kubectl get deployments -L app | awk '{print $6}' | sed '1d')
app_labels=($app_labels)

# for i in "${!app_labels[@]}"; do
#     echo "$i=>${app_labels[i]}"
# done

# version_labels=$(kubectl get deployments -L version | awk '{print $6}' | sed '1d')
# version_labels=($version_labels)

# for i in "${!version_labels[@]}"; do
#     echo "$i=>${version_labels[i]}"
# done

# if [ ${#app_labels[@]} != ${#version_labels[@]} ]; then
#     echo "something went wrong, number of labels are different"
#     exit 1
# fi

for i in "${!app_labels[@]}"; do
    output="wasme ${command} istio ${filter_name} \
    --id=${id}\
    --namespace=${namespace}\
    --labels=app=${app_labels[i]}\
    --config=\"{'name': '${app_labels[i]}'}\""

    echo $output
done

# version_label=$(kubectl get deployments -L version | awk '{print $6}' | sed '1d')
# echo ${version_label}

# for l in $app_labels; do
#     echo "wasme deploy filter ${l} ${filter_name}"
# done
