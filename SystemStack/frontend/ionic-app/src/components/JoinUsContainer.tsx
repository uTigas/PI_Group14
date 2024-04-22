import './ExploreContainer.css';

interface ContainerProps {
  uri: string;
}

const JoinUsContainer: React.FC<ContainerProps> = ({ uri }) => {
  return (
    <div className="container">
      <strong>Since 2024</strong>
      <p>Value your privacy? <a href={uri + "login"}>Join Us.</a></p>
    </div>
  );
};

export default JoinUsContainer;
