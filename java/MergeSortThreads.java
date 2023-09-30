import java.util.Arrays;
import java.util.Random;

public class MergeSortThreads {

    static final double DOUBLE_MIN = 1.0;
    static final double DOUBLE_MAX = 1000.0;

    class ThreadData {
        double[] array;
        double avg;

        ThreadData(double[] array) {
            this.array = array;
        }
    }

    class MergedThreadData {
        ThreadData thData0;
        ThreadData thData1;
        ThreadData thMergedData;
    }

    public static void main(String[] args) {
        
        if (args.length != 1) {
            System.err.println("There must only be one parameter");
            System.exit(-1);
        }

        int arraySize = Integer.parseInt(args[0]);

        MergeSortThreads instance = new MergeSortThreads();

        // Creating the array and filling it with random double values
        double[] array = new double[arraySize];
        for (int i = 0; i < arraySize; i++) {
            array[i] = instance.generateRandomDouble(DOUBLE_MIN, DOUBLE_MAX);
        }

        // Splitting the array into two halves
        double[] firstHalf = Arrays.copyOfRange(array, 0, arraySize / 2);
        double[] secondHalf = Arrays.copyOfRange(array, arraySize / 2, arraySize);

        // Creating threads to sort each half
        ThreadData firstHalfData = instance.new ThreadData(firstHalf);
        ThreadData secondHalfData = instance.new ThreadData(secondHalf);

        Thread sortThread1 = new Thread(instance.new SortThread(firstHalfData));
        Thread sortThread2 = new Thread(instance.new SortThread(secondHalfData));
        
        double startTime = System.currentTimeMillis(); // get the start time
        sortThread1.start();
        sortThread2.start();

        try {
            sortThread1.join();
            sortThread2.join();
            
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        double endTime = System.currentTimeMillis();
    
        System.out.println("Sorting is done in " + (endTime - startTime) + "ms when two threads are used.");
       
        startTime = System.currentTimeMillis();

        // Merging the two sorted halves
        MergedThreadData mergedData = instance.new MergedThreadData();
        mergedData.thData0 = firstHalfData;
        mergedData.thData1 = secondHalfData;
        mergedData.thMergedData = instance.new ThreadData(new double[arraySize]);

        Thread mergeThread = new Thread(instance.new MergeThread(mergedData));
        mergeThread.start();
        
        try {
            mergeThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        
        endTime = System.currentTimeMillis();

        System.out.println("Sorting is done in " + (endTime - startTime) + "ms when one thread is used.");
        
        if (arraySize < 10) {
            System.out.println("Average of array: " + mergedData.thMergedData.avg);
            for (int i = 0; i < arraySize; ++i) {
                System.out.println(mergedData.thMergedData.array[i]);
            }
        } else {
            System.out.println("Average of array:  " + mergedData.thMergedData.avg);
            for (int i = 0; i < 10; ++i) {
                System.out.println(mergedData.thMergedData.array[i]);
            }
        }
        

    }

    void printArray(double[] array) {
        for (double val : array) {
            System.out.println(val);
        }
    }

    double generateRandomDouble(double min, double max) {
        Random r = new Random();
        return min + (max - min) * r.nextDouble();
    }

    void copyArray(double[] src, double[] dest, int length) {
        System.arraycopy(src, 0, dest, 0, length);
    }

    void selectionSort(double[] array) {
        int n = array.length;
        for (int i = 0; i < n - 1; i++) {
            int min_idx = i;
            for (int j = i + 1; j < n; j++) {
                if (array[j] < array[min_idx]) {
                    min_idx = j;
                }
            }
            double temp = array[min_idx];
            array[min_idx] = array[i];
            array[i] = temp;
        }
    }

    class SortThread implements Runnable {
        ThreadData data;

        SortThread(ThreadData data) {
            this.data = data;
        }

        @Override
        public void run() {
            selectionSort(data.array);
            double sum = 0.0;
            for(double d : data.array) {
                sum += d;
            }
            data.avg = sum / data.array.length;
        }
    }

    class MergeThread implements Runnable {
        MergedThreadData data;

        MergeThread(MergedThreadData data) {
            this.data = data;
        }

        @Override
        public void run() {
            double[] mergedArray = new double[data.thData0.array.length + data.thData1.array.length];
            System.arraycopy(data.thData0.array, 0, mergedArray, 0, data.thData0.array.length);
            System.arraycopy(data.thData1.array, 0, mergedArray, data.thData0.array.length, data.thData1.array.length);

            data.thMergedData.array = mergedArray;

            double avg = (data.thData0.avg + data.thData1.avg) / 2;
            data.thMergedData.avg = avg;
        }
    }
}
