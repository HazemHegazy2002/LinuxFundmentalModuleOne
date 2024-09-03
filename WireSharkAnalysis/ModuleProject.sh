#!/bin/bash

# Main function to execute the script
function main() {
    echo "----- Network Traffic Analysis Report -----"  # Print a header for the report
    echo ""
    PcapFilePath="$1"  # Assign the first argument to the variable PcapFilePath
    count_packets "$PcapFilePath"  # Call function to count packets
    top_ip_addresses "$PcapFilePath"  # Call function to get top IP addresses
    echo ""
    echo "----- End of Report -----"  # Print a footer for the report
}

# Function to count various types of packets in the pcap file
function count_packets() {
    PcapFilePath="$1"  # Get the pcap file path from the function argument

    # Use tshark to read the pcap file and count total packets
    Total_packetcount=$(tshark -r "$PcapFilePath" | wc -l)

    # Use tshark with a display filter to count HTTP packets
    HTTP_packetscount=$(tshark -r "$PcapFilePath" -Y http | wc -l)

    # Use tshark with a display filter to count HTTPS/TLS packets
    TLS_packetcount=$(tshark -r "$PcapFilePath" -Y tls | wc -l)

    # Output the results
    echo "Total number of packets: $Total_packetcount"
    echo "Total number of HTTP packets: $HTTP_packetscount"
    echo "Total number of HTTPS/TLS packets: $TLS_packetcount"
}

# Function to extract and display the top 5 source and destination IP addresses
function top_ip_addresses() {
     PcapFilePath="$1"  # Get the pcap file path from the function argument

    # Extract top 5 source IP addresses with packet counts
    # -T fields: specify output format as fields
    # -e ip.src: extract the source IP addresses
    # sort | uniq -c: sort IPs and count occurrences
    # sort -nr: sort by count in numerical reverse order
    # head -n 5: get the top 5 results
    top_source_ips=$(tshark -r "$PcapFilePath" -T fields -e ip.src | sort | uniq -c | sort -nr | head -n 5)

    # Extract top 5 destination IP addresses with packet counts
    top_destination_ips=$(tshark -r "$PcapFilePath" -T fields -e ip.dst | sort | uniq -c | sort -nr | head -n 5)

    # Output the top 5 source IP addresses and their counts
    echo ""
    echo "Top 5 Source IP Addresses and their counts:"
    echo "$top_source_ips" | awk '{print $2 ": " $1 " Packets"}'  # Format output with "Packets" appended

    # Output the top 5 destination IP addresses and their counts
    echo ""
    echo "Top 5 Destination IP Addresses and their counts:"
    echo "$top_destination_ips" | awk '{print $2 ": " $1 " Packets"}'  # Format output with "Packets" appended
}

# Call the main function with the first script argument (pcap file path)
main "$1"
